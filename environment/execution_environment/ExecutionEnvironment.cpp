#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../util/api/APIDriver.h"

QString GWSExecutionEnvironment::ENTITY_BIRTH_PROP = "birth";
QString GWSExecutionEnvironment::ENTITY_DEATH_PROP = "death";
QString GWSExecutionEnvironment::ENTITY_RUNNING_PROP = "running";
QString GWSExecutionEnvironment::STARTED_SIMULATION_TIME = "started_simulation_time";
QString GWSExecutionEnvironment::STARTED_REAL_TIME = "started_real_time";
QString GWSExecutionEnvironment::ENDED_SIMULATION_TIME = "ended_simulation_time";
QString GWSExecutionEnvironment::ENDED_REAL_TIME = "ended_real_time";
QString GWSExecutionEnvironment::CURRENT_TICK_TIME = "current_tick_time";
QString GWSExecutionEnvironment::WAIT_FOR_ME_PROP = "wait_for_me";

GWSExecutionEnvironment* GWSExecutionEnvironment::globalInstance(){
    static GWSExecutionEnvironment instance;
    return &instance;
}

GWSExecutionEnvironment::GWSExecutionEnvironment() :
    tick_time_window( GWSApp::globalInstance()->getConfiguration().value("tick_time_window").toDouble( 4 ) * 999 ) ,
    max_entity_amount_per_tick( GWSApp::globalInstance()->getConfiguration().value("tick_entity_amount").toDouble( 500 ) ) ,
    GWSEnvironment() {
    qInfo() << "ExecutionEnvironment created";
    this->running_entities = new GWSObjectStorage();
    this->setProperty( ENTITY_BIRTH_PROP , GWSApp::globalInstance()->getConfiguration().value( "start" ).toDouble( -1 ) );
    this->setProperty( ENTITY_DEATH_PROP , GWSApp::globalInstance()->getConfiguration().value( "end" ).toDouble( INFINITY ) );
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSExecutionEnvironment::~GWSExecutionEnvironment(){
    this->running_entities->deleteLater();
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSExecutionEnvironment::serialize() const{
    QJsonObject json = GWSEnvironment::serialize();
    json.insert( "running_amount" , this->getRunningAmount() );
    json.insert( "ticks_amount" , this->getTicksAmount() );
    json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , (qint64)GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSExecutionEnvironment::containsEntity( QSharedPointer<GWSEntity> entity) const{
    return this->running_entities->contains( entity );
}

int GWSExecutionEnvironment::getRunningAmount() const{
    return this->running_entities->getAmount();
}

QList< QSharedPointer< GWSEntity > > GWSExecutionEnvironment::getRunning() const {
    return this->running_entities->getByClass<GWSEntity>( GWSEntity::staticMetaObject.className() );
}
bool GWSExecutionEnvironment::isRunning() const{
    return this->getProperty( ENTITY_RUNNING_PROP ).toBool( false );
}

int GWSExecutionEnvironment::getTicksAmount() const{
    return this->executed_ticks_amount;
}

/**********************************************************************
 ENTITY METHODS
**********************************************************************/

void GWSExecutionEnvironment::registerEntity( QSharedPointer<GWSEntity> entity){

    // If already registered
    if( entity.isNull() || entity->getEnvironments().contains( this ) || entity->getProperty( GWSExecutionEnvironment::ENTITY_BIRTH_PROP ).isUndefined() ){
        return;
    }

    // Whichever its BIRTH_DATE is, register the entity in this environment,
    // It needs to be executed in the future, so set its INTERNAL_TIME to that future
    qint64 entity_time = entity->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( -1 );
    if( entity_time <= 0 ){
        entity_time = entity->getProperty( GWSExecutionEnvironment::ENTITY_BIRTH_PROP ).toDouble( -1 );
        entity->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , entity_time );
    }

    entity->setProperty( ENTITY_RUNNING_PROP , true );
    entity->incrementBusy();

    // Store as running
    GWSEnvironment::registerEntity( entity );
    this->running_entities->add( entity );

    entity->decrementBusy();
    emit entity->entityStartedSignal();

}

void GWSExecutionEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity ){

    if( entity.isNull() || !entity->getEnvironments().contains( this ) ){
        return;
    }

    entity->setProperty( ENTITY_RUNNING_PROP , false );
    entity->incrementBusy();

    // Remove from running lists
    GWSEnvironment::unregisterEntity( entity );
    this->running_entities->remove( entity );

    entity->decrementBusy();
    emit entity->entityEndedSignal();

}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSExecutionEnvironment::run(){

    if( this->isRunning() ){
        qDebug() << QString("%1 is already running").arg( this->metaObject()->className() );
        return;
    }

    this->setProperty( ENTITY_RUNNING_PROP , true );
    this->setProperty( STARTED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    this->setProperty( STARTED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() );

    emit this->runningExecutionSignal();

    // Set timeout not to have infinite simulations
    int timeout = GWSApp::globalInstance()->getConfiguration().value( "timeout" ).toInt( 5 );
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

    QList< QSharedPointer<GWSEntity> > currently_running_entities = this->getRunning();

    if( currently_running_entities.isEmpty() ){
        GWSApp::globalInstance()->exit( 0 );
        this->stop();
        return;
    }

    // Shuffle list
    //std::random_shuffle( currently_running_entities.begin() , currently_running_entities.end() );

    // Get current datetime in simulation
    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();

    // If simulation end_time reached
    if( current_datetime > this->getProperty( ENTITY_DEATH_PROP ).toDouble() ){
        GWSApp::globalInstance()->exit( 0 );
        this->stop();
        return;
    }

    // Wait for entities that are delayed (if WAIT_FOR_ME).
    // Get min tick time and add some threshold to execute the entities that are more delayed.
    qint64 min_tick = -1;
    QSharedPointer<GWSEntity> who_is_min_tick;
    bool entities_to_tick = false;
    int ticked_entities = 0;

    foreach( QSharedPointer<GWSEntity> entity , currently_running_entities ){
        if( ( entity.isNull() || entity->isBusy() ) && !entity->getProperty( WAIT_FOR_ME_PROP ).toBool( false ) ) {
            continue;
        }

        qint64 entity_time = entity->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( current_datetime );

        if( entity_time > 0 && min_tick <= 0 ){ min_tick = entity_time; }
        if( entity_time > 0 && min_tick > 0 ){ min_tick = qMin( min_tick , entity_time ); }
        if( min_tick == entity_time ){ who_is_min_tick = entity; }
        entities_to_tick = true;
    }

    if( entities_to_tick ){

        // Store min tick
        if( min_tick <= 0 || min_tick > current_datetime ){ min_tick = current_datetime; }
        if( min_tick < current_datetime ){
            GWSTimeEnvironment::globalInstance()->setDatetime( min_tick );
        }

        qint64 limit = min_tick + this->tick_time_window; // Add threshold, otherwise only the minest_tick entity is executed
        foreach( QSharedPointer<GWSEntity> entity , currently_running_entities ){

            if( entity.isNull() || entity->isBusy() ) {
                continue;
            }

            qint64 entity_next_tick = entity->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( -1 );

            if ( !entity->getProperty( GWSExecutionEnvironment::ENTITY_DEATH_PROP ).isUndefined() && min_tick >= entity->getProperty( GWSExecutionEnvironment::ENTITY_DEATH_PROP ).toDouble() ){
                this->unregisterEntity( entity );
                continue;
            }

            if( entity && !entity->deleted && !entity->isBusy() && entity_next_tick <= limit ){

                if( entity_next_tick <= 0 ){ entity->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , min_tick ); }

                // Call behave through tick for it to be executed in the entity's thread (important to avoid msec < 100)
                entity->incrementBusy(); // Increment here, Decrement after entity Tick()

                //entity->tick();
                QtConcurrent::run( entity.data() , &GWSEntity::tick );

                if( ++ticked_entities >= this->max_entity_amount_per_tick ){
                    who_is_min_tick = entity;
                    break;
                }
            }
        }
    }

    QString message = QString("Ticked %1 , Entities %2 / %3 , Entities to tick %4 ,  Active threads %5 / %6" )
            .arg( min_tick <= 0 ? "Waiting" : QDateTime::fromMSecsSinceEpoch( min_tick ).toString("yyyy-MM-ddTHH:mm:ss") )
            .arg( ticked_entities )
            .arg( currently_running_entities.size() )
            .arg( entities_to_tick ? "True" : "False" )
           // .arg( who_is_min_tick.isNull() ? "" : who_is_min_tick->getUID() )
            .arg( QThreadPool::globalInstance()->activeThreadCount() )
            .arg( QThreadPool::globalInstance()->maxThreadCount() );

    qInfo() << message;

    emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal(
                QJsonObject({ { "message" , message } , { GWSTimeEnvironment::INTERNAL_TIME_PROP , min_tick } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );

    emit this->tickEndedSignal( this->executed_ticks_amount++ );

    // Call again this function
    if( this->isRunning() ) {

        // DO NOT USE QTCONCURRENT FOR THIS.
        // OTHERWISE IT WAITS FOR ALL PENDING ENTITIES TO TICK, GETS SLOW AND QUITE SYNCRHONOUS
        // DIRECTLY USE QTIMER::SINGLESHOT
        qint64 next_tick_in = qMax( 10.0 , 1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed() );

        // WAIT SOME MORE
        if( ticked_entities <= 0 ){
            next_tick_in = next_tick_in*2;
        }

        // DO NOT ADVANCE IN TIME
        if( min_tick <= 0 ){
            next_tick_in = 1000;
            GWSTimeEnvironment::globalInstance()->setDatetime( current_datetime - (1000 * GWSTimeEnvironment::globalInstance()->getTimeSpeed() ) );
        }

        QTimer::singleShot( next_tick_in , this , &GWSExecutionEnvironment::tick );
    }
}

void GWSExecutionEnvironment::stop(){

    if( !this->isRunning() ){ return; }

    this->setProperty( ENTITY_RUNNING_PROP , false );
    this->setProperty( ENDED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
    this->setProperty( ENDED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() );

    emit this->stoppingExecutionSignal();
}
