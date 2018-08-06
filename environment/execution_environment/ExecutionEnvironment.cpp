#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../util/parallelism/ParallelismController.h"

QString GWSExecutionEnvironment::RUNNING_PROP = "running";

GWSExecutionEnvironment* GWSExecutionEnvironment::globalInstance(){
    static GWSExecutionEnvironment instance;
    return &instance;
}

GWSExecutionEnvironment::GWSExecutionEnvironment() : GWSEnvironment() {
    qInfo() << "ExecutionEnvironment created";
    this->running_agents = new GWSObjectStorage( this );
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

int GWSExecutionEnvironment::getRunningAgentsAmount() const{
    return this->running_agents->getAmount();
}

QList<GWSAgent*> GWSExecutionEnvironment::getRunningAgents() const {
    QList <GWSAgent*> list;
    foreach (GWSObject* obj, this->running_agents->getByClass( GWSAgent::staticMetaObject.className() ) ) {
        list.append( dynamic_cast<GWSAgent*>( obj ) );
    }
    return list;
}

template <class T> QList<T*> GWSExecutionEnvironment::getRunningAgentsByClass( QString class_name ) const{
    return this->running_agents->getByClass<T>( class_name );
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

void GWSExecutionEnvironment::registerAgent(GWSAgent *agent){

    agent->incrementBusy();

    // Create agent's own timer to schedule its slots
    if( agent->timer ){ agent->timer->deleteLater(); }
    agent->timer = new QTimer( agent ); // Set its parent to this agent, Really improves speed
    agent->timer->setSingleShot( true ); // Set single shot, really improves speed

    if( !agent || agent->deleted ){ return; }

    // Store as running
    GWSEnvironment::registerAgent( agent );
    this->running_agents->add( agent );

    // Calculate when to start the agent according to its next_tick_datetime
    qint64 msecs = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent ) - GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    if( msecs < 0 ){
        msecs = 0;
    }

    // Balance agents between threads
    // Non blocking method (if next_tick_datetime is invalid, msecs will be 1000 and it will be run instantly)
    agent->timer->singleShot( msecs / GWSTimeEnvironment::globalInstance()->getTimeSpeed() , Qt::CoarseTimer , [this , agent](){

        if( !agent || agent->deleted ){ return; }

        // Balance between threads. Only QObjects without parent can be moved. Children must stay in parents thread
        // CAUTION! Very slow operation
        if( !agent->parent() ){
            qDebug() << QString("Moving agent %1 %2 to a parallel thread").arg( agent->metaObject()->className() ).arg( agent->getId() );
            agent->moveToThread( GWSParallelismController::globalInstance()->getThread( qrand() ) );
            if( agent->timer ){ agent->timer->moveToThread( agent->thread() ); }
        }

        // Run agent
        agent->setProperty( GWSAgent::RUNNING_PROP , true );
        agent->decrementBusy();
        emit agent->agentStartedSignal();

        qDebug() << QString("Agent %1 %2 running").arg( agent->metaObject()->className() ).arg( agent->getId() );
    });
}

void GWSExecutionEnvironment::unregisterAgent(GWSAgent *agent){

    agent->incrementBusy();
    agent->setProperty( GWSAgent::RUNNING_PROP , false );

    // Remove from running lists
    GWSEnvironment::unregisterAgent( agent );
    this->running_agents->remove( agent );

    // Stop agent
    qDebug() << QString("Agent %1 %2 stopped").arg( agent->metaObject()->className() ).arg( agent->getId() );

    // Delete its timer to schedule slots
    if( agent->timer ){
        agent->timer->deleteLater();
        agent->timer = Q_NULLPTR;
    }

    agent->decrementBusy();
    emit agent->agentEndedSignal();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSExecutionEnvironment::run(){

    if( this->isRunning() ){ qDebug() << QString("%1 is already running").arg( this->metaObject()->className() ); return; }

    this->setProperty( GWSAgent::RUNNING_PROP , true );

    qInfo() << QString("Running %1").arg( this->metaObject()->className() );
    emit GWSApp::globalInstance()->pushDataSignal( "message" , "Running simulation" );
    this->timer = new QTimer();
    this->timer->singleShot( 1000 , Qt::CoarseTimer , this , &GWSExecutionEnvironment::tick );

    emit this->runningExecutionSignal();
}

void GWSExecutionEnvironment::behave(){

    QList<GWSAgent*> currently_running_agents = this->getRunningAgents();

    if( currently_running_agents.isEmpty() && !GWSApp::globalInstance()->property( "live" ).toBool() ){
        qInfo() << QString("%1 has no agents to run. Finising Simulation.").arg( this->metaObject()->className() );
        this->stop();
        GWSApp::exit( 0 );
    }

    // Wait for agents that are delayed.
    // Get min tick time and add some threshold to execute the agents that are more delayed.
    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    qint64 min_tick = current_datetime;
    bool agents_to_tick = false;
    int ticked_agents = 0;

    foreach( GWSAgent* agent , currently_running_agents ){
        if( agent && !agent->isBusy() ){
            agents_to_tick = true;
            qint64 agent_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent );
            if( agent_time > 0 ){
                min_tick = qMin( min_tick , agent_time );
            }
        }
    }

    if( agents_to_tick ){

        // Update TimeEnvironment datetime, before calling agents
        GWSTimeEnvironment::globalInstance()->setDatetime( min_tick + 1000 );

        qint64 limit = min_tick + this->tick_time_window; // Add threshold, otherwise only the minest_tick agent is executed
        foreach( GWSAgent* agent , currently_running_agents ){

            qint64 agent_next_tick = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent );
            if( agent && !agent->deleted && agent->isRunning() && !agent->isBusy() && agent_next_tick <= limit ){

                // Set agent to advance to last min_tick, in case it was set to 0
                GWSTimeEnvironment::globalInstance()->setAgentInternalTime( agent , qMax( agent_next_tick , min_tick ) );

                // Call behave through behaveWrapper for it to be executed in the agents thread (important to avoid msec < 1000)
                agent->timer->singleShot( 10 + (qrand() % 100) , agent , &GWSAgent::tick );

                ticked_agents++;
            }
        }
    }

    // Calculate to call again this function in (cycleFrequency - spent time) time
    if( this->isRunning() ){
        this->timer->singleShot( (1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed()) , Qt::CoarseTimer , this , &GWSExecutionEnvironment::tick );
    }

    qInfo() << QString("Environment should be : %1 / Executing tick : %2")
               .arg( QDateTime::fromMSecsSinceEpoch( current_datetime ).toString() )
               .arg( QDateTime::fromMSecsSinceEpoch( min_tick ).toString() );
    qInfo() << QString("Ticked agents : %1 / Running agents : %2")
               .arg( ticked_agents )
               .arg( currently_running_agents.size() );

    emit this->tickEndedSignal( this->executed_ticks_amount++ );
}

void GWSExecutionEnvironment::stop(){

    if( !this->isRunning() ){ return; }
    this->setProperty( GWSAgent::RUNNING_PROP , false );

    qInfo() << QString("Stopping %1").arg( this->metaObject()->className() );
    this->timer->deleteLater();
    this->timer = Q_NULLPTR;

    emit this->stoppingExecutionSignal();
}
