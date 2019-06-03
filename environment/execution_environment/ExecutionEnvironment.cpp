#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../util/api/APIDriver.h"

QString GWSExecutionEnvironment::AGENT_BIRTH_PROP = "birth";
QString GWSExecutionEnvironment::AGENT_DEATH_PROP = "death";
QString GWSExecutionEnvironment::AGENT_RUNNING_PROP = "running";
QString GWSExecutionEnvironment::STARTED_SIMULATION_TIME = "started_simulation_time";
QString GWSExecutionEnvironment::STARTED_REAL_TIME = "started_real_time";
QString GWSExecutionEnvironment::ENDED_SIMULATION_TIME = "ended_simulation_time";
QString GWSExecutionEnvironment::ENDED_REAL_TIME = "ended_real_time";
QString GWSExecutionEnvironment::CURRENT_TICK_TIME = "current_tick_time";

GWSExecutionEnvironment* GWSExecutionEnvironment::globalInstance(){
    static GWSExecutionEnvironment instance;
    return &instance;
}

GWSExecutionEnvironment::GWSExecutionEnvironment() :
    tick_time_window( GWSApp::globalInstance()->getConfiguration().value("tick_threshold").toDouble( 5 ) * 9999 ) ,
    max_agent_amount_per_tick( GWSApp::globalInstance()->getConfiguration().value("max_agents_per_tick").toDouble( 500 ) ) ,
    GWSEnvironment() {
    qInfo() << "ExecutionEnvironment created";
    this->running_agents = new GWSObjectStorage();
    this->setProperty( AGENT_BIRTH_PROP , GWSApp::globalInstance()->getConfiguration().value( "start" ).toDouble( -1 ) );
    this->setProperty( AGENT_DEATH_PROP , GWSApp::globalInstance()->getConfiguration().value( "end" ).toDouble( INFINITY ) );
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSExecutionEnvironment::~GWSExecutionEnvironment(){
    this->running_agents->deleteLater();
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSExecutionEnvironment::serialize() const{
    QJsonObject json = GWSEnvironment::serialize();
    json.insert( "running_amount" , this->getRunningAgentsAmount() );
    json.insert( "ticks_amount" , this->getTicksAmount() );
    json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , (qint64)GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
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

QList< QSharedPointer< GWSAgent > > GWSExecutionEnvironment::getRunningAgents() const {
    return this->running_agents->getByClass<GWSAgent>( GWSAgent::staticMetaObject.className() );
}

bool GWSExecutionEnvironment::isRunning() const{
    return this->getProperty( AGENT_RUNNING_PROP ).toBool( false );
}

int GWSExecutionEnvironment::getTicksAmount() const{
    return this->executed_ticks_amount;
}

/**********************************************************************
 AGENT METHODS
**********************************************************************/

void GWSExecutionEnvironment::registerAgent( QSharedPointer<GWSAgent> agent){

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) || agent->getProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP ).isNull() ){
        return;
    }

    // Whichever its BIRTH_DATE is, register the agent in this environment,
    // It needs to be executed in the future, so set its INTERNAL_TIME to that future
    qint64 agent_time = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( -1 );
    if( agent_time <= 0 ){
        qint64 agent_birth = agent->getProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP ).toDouble( -1 );
        agent->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , agent_birth );
    }
    agent->setProperty( AGENT_RUNNING_PROP , true );
    agent->incrementBusy();

    // Store as running
    GWSEnvironment::registerAgent( agent );
    this->running_agents->add( agent );

    agent->decrementBusy();
    emit agent->agentStartedSignal();

    //qDebug() << QString("Agent %1 %2 running").arg( agent->metaObject()->className() ).arg( agent->getId() );
}

void GWSExecutionEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

    if( agent.isNull() || !agent->getEnvironments().contains( this ) ){
        return;
    }

    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    agent->setProperty( AGENT_DEATH_PROP , current_datetime );
    agent->setProperty( AGENT_RUNNING_PROP , false );

    agent->incrementBusy();

    // Remove from running lists
    GWSEnvironment::unregisterAgent( agent );
    this->running_agents->remove( agent );

    // Stop agent
    //qDebug() << QString("Agent %1 %2 stopped").arg( agent->metaObject()->className() ).arg( agent->getId() );

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

    this->setProperty( AGENT_RUNNING_PROP , true );
    this->setProperty( STARTED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    this->setProperty( STARTED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() );

    emit this->runningExecutionSignal();

    // Set timeout not to have infinite simulations
    int timeout = GWSApp::globalInstance()->getConfiguration().value( "timeout" ).toInt( 60 );
    QTimer::singleShot( timeout * 1000 , []{
        GWSApp::globalInstance()->exit( -1 );
    });

    // Make Scenario to listen
    GWSAPIDriver::globalInstance()->GET(
                QString("https://history.geoworldsim.com/api/scenario/%1/socket").arg( GWSApp::globalInstance()->getAppId() ) ,
                []( QNetworkReply* reply ){
                   reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );
    });

    // Start ticking
    QtConcurrent::run([ this ] { this->tick(); });
}

void GWSExecutionEnvironment::behave(){

    QList< QSharedPointer<GWSAgent> > currently_running_agents = this->getRunningAgents();

    if( currently_running_agents.isEmpty() ){
        this->stop();
        QTimer::singleShot( 10*1000 , [](){ GWSApp::exit( 0 ); });
        return;
    }

    // Shuffle list
    std::random_shuffle( currently_running_agents.begin() , currently_running_agents.end() );

    // Get current datetime in simulation
    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();

    // If simulation end_time reached
    if( current_datetime > this->getProperty( AGENT_DEATH_PROP ).toDouble() ){
        this->stop();
        QTimer::singleShot( 10*1000 , [](){ GWSApp::exit( 0 ); });
        return;
    }

    // Wait for agents that are delayed (if WAIT_FOR_ME).
    // Get min tick time and add some threshold to execute the agents that are more delayed.
    qint64 min_tick = -1;
    QSharedPointer<GWSAgent> who_is_min_tick;
    bool agents_to_tick = false;
    int ticked_agents = 0;

    foreach( QSharedPointer<GWSAgent> agent , currently_running_agents ){
        if( agent.isNull() || agent->isBusy() ) {
            continue;
        }
        /*else
            if( !agent->getProperty( "scenario_id" ).isNull() || agent->getProperty( "scenario_id" ).toString() != GWSApp::globalInstance()->getAppId()  ){
            continue;
        }*/

        qint64 agent_time = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( current_datetime );

        if( agent_time > 0 && min_tick <= 0 ){ min_tick = agent_time; }
        if( agent_time > 0 && min_tick > 0 ){ min_tick = qMin( min_tick , agent_time ); }
        if( min_tick == agent_time ){ who_is_min_tick = agent; }
        agents_to_tick = true;
    }

    if( agents_to_tick ){

        // Store min tick
        if( min_tick <= 0 ){  min_tick = current_datetime; }
        this->last_tick_with_agents = min_tick;
        GWSTimeEnvironment::globalInstance()->setDatetime( min_tick );

        qint64 limit = min_tick + this->tick_time_window; // Add threshold, otherwise only the minest_tick agent is executed
        foreach( QSharedPointer<GWSAgent> agent , currently_running_agents ){

            if( agent.isNull() || agent->isBusy() ) {
                continue;
            }

            qint64 agent_next_tick = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( -1 );

            if ( !agent->getProperty( GWSExecutionEnvironment::AGENT_DEATH_PROP ).isNull() && current_datetime >= agent->getProperty( GWSExecutionEnvironment::AGENT_DEATH_PROP ).toDouble() ){
                this->unregisterAgent( agent );
                continue;
            }

            if( agent && !agent->deleted && !agent->isBusy() && agent_next_tick <= limit ){

                if( agent_next_tick <= 0 ){ agent->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , min_tick ); }

                // Call behave through tick for it to be executed in the agents thread (important to avoid msec < 100)
                agent->incrementBusy(); // Increment here, Decrement after agent Tick()

                QtConcurrent::run( agent.data() , &GWSAgent::tick );

                if( ++ticked_agents >= this->max_agent_amount_per_tick ){
                    who_is_min_tick = agent;
                    break;
                }
            }
        }
    }

    QString message = QString("Ticked %1 , Agents %2 / %3 , Min tick %4" )
            .arg( min_tick <= 0 ? "Waiting" : QDateTime::fromMSecsSinceEpoch( min_tick ).toString("yyyy-MM-ddTHH:mm:ss") )
            .arg( ticked_agents )
            .arg( currently_running_agents.size() )
            .arg( who_is_min_tick ? who_is_min_tick->getUID() : "" );

    qInfo() << message;

    emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal(
                QJsonObject({ { "message" , message } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );

    emit this->tickEndedSignal( this->executed_ticks_amount++ );

    // Call again this function
    if( this->isRunning() ) {

        // DO NOT USE QTCONCURRENT FOR THIS.
        // OTHERWISE IT WAITS FOR ALL PENDING AGENTS TO TICK, GETS SLOW AND QUITE SYNCRHONOUS
        // DIRECTLY USE QTIMER::SINGLESHOT
        qint64 next_tick_in = qMax( 10.0 , 1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed() );

        // DO NOT ADVANCE IN TIME
        if( !agents_to_tick ){
            next_tick_in = 5 * 1000;
            GWSTimeEnvironment::globalInstance()->setDatetime( this->last_tick_with_agents - (1000 * GWSTimeEnvironment::globalInstance()->getTimeSpeed() ) );
        }

        QTimer::singleShot( next_tick_in , this , &GWSExecutionEnvironment::tick );
    }
}

void GWSExecutionEnvironment::stop(){

    if( !this->isRunning() ){ return; }

    this->setProperty( AGENT_RUNNING_PROP , false );
    this->setProperty( ENDED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    this->setProperty( ENDED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() );

    emit this->stoppingExecutionSignal();
}
