#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../util/parallelism/ParallelismController.h"

QString GWSExecutionEnvironment::RUNNING_PROP = "running";

GWSExecutionEnvironment* GWSExecutionEnvironment::globalInstance(){
    static GWSExecutionEnvironment instance;
    return &instance;
}

GWSExecutionEnvironment::GWSExecutionEnvironment() : GWSEnvironment() {
    qInfo() << "ExecutionEnvironment created";
    this->running_agents = new GWSObjectStorage();
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSExecutionEnvironment::~GWSExecutionEnvironment(){
    this->running_agents->deleteLater();
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSExecutionEnvironment::serialize() const{
    QJsonObject json = GWSEnvironment::serializeMini();
    json.insert( "running_amount" , this->getRunningAgentsAmount() );
    json.insert( "ticks_amount" , this->getTicksAmount() );
    json.insert( "time" , (qint64)GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSExecutionEnvironment::containsAgent( QSharedPointer<GWSAgent> agent) const{
    return this->running_agents->contains( agent );
}

int GWSExecutionEnvironment::getRunningAgentsAmount() const{
    return this->running_agents->getAmount();
}

QList< QSharedPointer<GWSAgent> > GWSExecutionEnvironment::getRunningAgents() const {
    QList< QSharedPointer<GWSAgent> > l;
    foreach( QSharedPointer<GWSObject> o , this->running_agents->getByClass( GWSAgent::staticMetaObject.className() ) ){
        l.append( o.dynamicCast<GWSAgent>() );
    }
    return l;
}

template <class T> QList< QSharedPointer<T> > GWSExecutionEnvironment::getRunningAgentsByClass( QString class_name ) const{
    return this->running_agents->getByClassCasted<T>( class_name );
}

bool GWSExecutionEnvironment::isRunning() const{
    return this->timer ? true : false;
}

int GWSExecutionEnvironment::getTicksAmount() const{
    return this->executed_ticks_amount;
}

/**********************************************************************
 AGENT METHODS
**********************************************************************/

void GWSExecutionEnvironment::registerAgent( QSharedPointer<GWSAgent> agent){

    // Not running
    if( !agent->getProperty( RUNNING_PROP ).toBool() ){
        return;
    }

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) ){
        return;
    }

    agent->incrementBusy();

    // Store as running
    GWSEnvironment::registerAgent( agent );
    this->running_agents->add( agent );

    agent->setProperty( GWSExecutionEnvironment::RUNNING_PROP , true );
    agent->decrementBusy();
    emit agent->agentStartedSignal();
    qDebug() << QString("Agent %1 %2 running").arg( agent->metaObject()->className() ).arg( agent->getId() );
}

void GWSExecutionEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent){

    if( agent.isNull() || !agent->getEnvironments().contains( this ) ){
        return;
    }

    agent->incrementBusy();
    agent->setProperty( GWSExecutionEnvironment::RUNNING_PROP , false );

    // Remove from running lists
    GWSEnvironment::unregisterAgent( agent );
    this->running_agents->remove( agent );

    // Stop agent
    qDebug() << QString("Agent %1 %2 stopped").arg( agent->metaObject()->className() ).arg( agent->getId() );

    agent->decrementBusy();
    emit agent->agentEndedSignal();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSExecutionEnvironment::run(){

    if( this->isRunning() ){
        qDebug() << QString("%1 is already running").arg( this->metaObject()->className() );
        return;
    }

    emit GWSApp::globalInstance()->sendAlertToSocketSignal( 0 , "Simulation running" , QString("Agents' execution started") );
    this->timer = new QTimer();
    this->timer->singleShot( 1000 , Qt::CoarseTimer , this , &GWSExecutionEnvironment::tick );

    emit this->runningExecutionSignal();
}

void GWSExecutionEnvironment::behave(){

    QList< QSharedPointer<GWSAgent> > currently_running_agents = this->getRunningAgents();

    if( currently_running_agents.isEmpty() && !GWSApp::globalInstance()->property( "live" ).toBool() ){
        emit GWSApp::globalInstance()->sendAlertToSocketSignal( 0 , "Simulation has no (more) agents" , QString("Simulation has no (more) agents to run, stopping and finishing.") );
        this->stop();
        GWSApp::exit( 0 );
    }

    // Wait for agents that are delayed (if WAIT_FOR_ME).
    // Get min tick time and add some threshold to execute the agents that are more delayed.
    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    qint64 min_tick = current_datetime;
    QString who_is_min_tick;
    bool agents_to_tick = false;
    int ticked_agents = 0;

    foreach( QSharedPointer<GWSAgent> agent , currently_running_agents ){
        if( agent ){
            qint64 agent_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent );
            if( agent_time <= 0 ){
                agents_to_tick = true;
            } else if( agent->getProperty( GWSTimeEnvironment::WAIT_FOR_ME_PROP ).toBool() || !agent->isBusy() ){
                min_tick = qMin( min_tick , agent_time );
                if( min_tick == agent_time ){ who_is_min_tick = agent->getId(); }
                agents_to_tick = true;
            }
        }
    }

    if( agents_to_tick ){

        qint64 limit = min_tick + this->tick_time_window; // Add threshold, otherwise only the minest_tick agent is executed
        QList< QFuture<void> > async_ticks;
        foreach( QSharedPointer<GWSAgent> agent , currently_running_agents ){

            qint64 agent_next_tick = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent );

            // If agent_tick is 0, set to now
            if( agent_next_tick <= 0 ){
                GWSTimeEnvironment::globalInstance()->setAgentInternalTime( agent , min_tick );
            }

            if( agent && !agent->deleted && !agent->isBusy() && agent_next_tick <= limit ){

                // Call behave through tick for it to be executed in the agents thread (important to avoid msec < 100)
                agent->incrementBusy(); // Increment here, Decrement after agent Tick()
                QtConcurrent::run( agent.data() , &GWSAgent::tick );
                //agent->tick();

                ticked_agents++;
            }
        }
    }

    // Calculate to call again this function in (cycleFrequency - spent time) time
    if( this->isRunning() ){
        this->timer->singleShot( (1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed()) , Qt::CoarseTimer , this , &GWSExecutionEnvironment::tick );
    }

    qInfo() << QString("Ticking %3 , Agents %1 / %2 , Min tick %4" )
               .arg( ticked_agents )
               .arg( currently_running_agents.size() )
               .arg( QDateTime::fromMSecsSinceEpoch( min_tick ).toString("yyyy-MM-ddTHH:mm:ss") )
               .arg( who_is_min_tick );

    emit GWSApp::globalInstance()->sendDataToSocketSignal( "time" , min_tick );

    emit this->tickEndedSignal( this->executed_ticks_amount++ );
}

void GWSExecutionEnvironment::stop(){

    if( !this->isRunning() ){ return; }

    emit GWSApp::globalInstance()->sendAlertToSocketSignal( 0 , "Simulation stopped" , QString("Agents execution stopped.") );
    this->timer->deleteLater();
    this->timer = Q_NULLPTR;

    emit this->stoppingExecutionSignal();
}
