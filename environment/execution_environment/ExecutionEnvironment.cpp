#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../util/parallelism/ParallelismController.h"

QString GWSExecutionEnvironment::RUNNING_PROP = "running";

GWSExecutionEnvironment* GWSExecutionEnvironment::globalInstance(){
    static GWSExecutionEnvironment instance;
    return &instance;
}

GWSExecutionEnvironment::GWSExecutionEnvironment() : GWSEnvironment() {
    qInfo() << "ExecutionEnvironment created";
}

GWSExecutionEnvironment::~GWSExecutionEnvironment(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSExecutionEnvironment::serializeMini(){
    QJsonObject json;
    json.insert( "running" , this->isRunning() );
    json.insert( "running_agents_amount" , this->running_agents.size() );
    json.insert( "executed_ticks" , (qint64)this->executed_ticks_amount );
    return json;
}

QJsonObject GWSExecutionEnvironment::serialize(){
    return this->serializeMini();
}

/**********************************************************************
 GETTERS
**********************************************************************/

int GWSExecutionEnvironment::getRunningAgents() const{
    return this->running_agents.size();
}

bool GWSExecutionEnvironment::isRunning() const{
    return this->timer ? true : false;
}

/**********************************************************************
 AGENT METHODS
**********************************************************************/

void GWSExecutionEnvironment::registerAgent(GWSAgent *agent){

    if( agent->isRunning() ){
        qDebug() << QString("Agent %1 %2 is already running").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return;
    }

    agent->incrementBusy();

    // Create agent's own timer to schedule its slots
    if( agent->timer ){ agent->timer->deleteLater(); }
    agent->timer = new QTimer( agent ); // Set its parent to this agent, Really improves speed
    agent->timer->setSingleShot( true ); // Set single shot, really improves speed

    if( !agent || agent->deleted ){ return; }

    // Store as running
    //this->mutex.lock();
    this->running_agents.append( agent );
    //this->mutex.unlock();

    // Calculate when to start the agent according to its next_tick_datetime
    qint64 msecs = agent->getProperty( GWSAgent::INTERNAL_TIME_PROP ).value<quint64>() - GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
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
    this->running_agents.removeAll( agent );

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

    if( this->timer ){ qDebug() << QString("%1 is already running").arg( this->metaObject()->className() ); return; }

    qInfo() << QString("Running %1").arg( this->metaObject()->className() );
    emit GWSApp::globalInstance()->pushDataSignal( "message" , "Running simulation" );
    this->timer = new QTimer();
    this->timer->singleShot( 1000 , Qt::CoarseTimer , this , &GWSExecutionEnvironment::tick );

    emit this->runningExecutionSignal();
}

void GWSExecutionEnvironment::tick(){

    //this->mutex.lock();
    QList<GWSAgent*> currently_running_agents = this->running_agents;
    //this->mutex.unlock();

    if( currently_running_agents.isEmpty() ){
        qInfo() << QString("%1 has no agents to run").arg( this->metaObject()->className() );
        this->stop(); return;
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
            if( agent->getInternalTime() > 0 ){
                min_tick = qMin( min_tick , agent->getInternalTime() );
            }
        }
    }

    if( agents_to_tick ){

        // Update TimeEnvironment datetime, before calling agents
        GWSTimeEnvironment::globalInstance()->setDatetime( min_tick + 1000 );

        qint64 limit = min_tick + this->min_tick_threshold; // Add threshold, otherwise only the minest_tick agent is executed
        foreach( GWSAgent* agent , currently_running_agents ){

            qint64 agent_next_tick = agent->getInternalTime();
            if( agent && !agent->deleted && agent->isRunning() && !agent->isBusy() && agent_next_tick <= limit ){

                    // Set agent to advance to last min_tick, in case it was set to 0
                    agent->setInternalTime( qMax( agent_next_tick , min_tick ) );

                    // Call behave through behaveWrapper for it to be executed in the agents thread (important to avoid msec < 1000)
                    agent->timer->singleShot( 10 + (qrand() % 100) , agent , &GWSAgent::tick );

                    ticked_agents++;
            }
        }
    }

    // Calculate to call again this function in (cycleFrequency - spent time) time
    if( this->timer ){
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

    if( !this->timer ){ return; }

    qInfo() << QString("Stopping %1").arg( this->metaObject()->className() );
    this->timer->deleteLater();
    this->timer = Q_NULLPTR;

    emit this->stoppingExecutionSignal();
}
