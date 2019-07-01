#include "ExecutionEnvironment.h"

#include <QCoreApplication>
#include <QDir>

#include "../../app/App.h"
#include "../../util/parallelism/ParallelismController.h"
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
    GWSEnvironment() ,
    max_entity_amount_per_tick( GWSApp::globalInstance()->getConfiguration().value( "tick_entity_amount" ).toDouble( -1 ) ) {

    this->environment_entities = new GWSObjectStorage();
    this->environment_entities->setObjectName( QString("%1").arg( this->metaObject()->className() ) );

    qInfo() << "ExecutionEnvironment created";
    this->setProperties( QJsonObject( {
        { ENTITY_BIRTH_PROP , GWSApp::globalInstance()->getConfiguration().value( "start" ).toDouble( -1 ) } ,
        { ENTITY_DEATH_PROP , GWSApp::globalInstance()->getConfiguration().value( "end" ).toDouble( Q_INFINITY ) }
    }) );
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );

}

GWSExecutionEnvironment::~GWSExecutionEnvironment(){
    foreach (GWSExecutionEnvironmentElement* p , this->parallel_executions.values()){
        p->deleteLater();
    }
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
    return this->environment_entities->contains( entity );
}

int GWSExecutionEnvironment::getRunningAmount() const{
    return this->environment_entities->getAmount();
}

QList< QSharedPointer< GWSEntity > > GWSExecutionEnvironment::getRunning() const {
   return this->environment_entities->getByClass<GWSEntity>( GWSEntity::staticMetaObject.className() );
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

void GWSExecutionEnvironment::registerEntity( QSharedPointer<GWSEntity> entity ){

    // If already registered
    if( entity.isNull() || entity->getEnvironments().contains( this ) || entity->getProperty( GWSExecutionEnvironment::ENTITY_BIRTH_PROP ).isNull() ){
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

    QThread* thread = entity->thread();
    GWSExecutionEnvironmentElement* parallel = this->parallel_executions.value( thread , Q_NULLPTR );
    if( !parallel ){
        parallel = new GWSExecutionEnvironmentElement(
                    thread ,
                    GWSApp::globalInstance()->getConfiguration().value("tick_time_window").toDouble( 4 ) * 999 ,
                    this->max_entity_amount_per_tick > 0 ? this->max_entity_amount_per_tick / GWSParallelismController::globalInstance()->getThreadsCount() : -1 );
        this->parallel_executions.insert( thread , parallel);
        parallel->is_running = this->isRunning();
    }
    parallel->running_storage.append( entity );
    emit this->environment_entities->addObjectSignal( entity );

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
    QThread* thread = entity->thread();
    GWSExecutionEnvironmentElement* parallel = this->parallel_executions.value( thread , Q_NULLPTR );
    if( parallel ){
        emit parallel->running_storage.removeAll( entity );
    }
    emit this->environment_entities->removeObjectSignal( entity );

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

    this->setProperties( QJsonObject( {
        { ENTITY_RUNNING_PROP , true } ,
        { STARTED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() } ,
        { STARTED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() }
        } ));

    foreach( GWSExecutionEnvironmentElement* p , this->parallel_executions.values() ) {
        p->is_running = true;
    }

    emit this->runningExecutionSignal();

    // Set timeout not to have infinite simulations
    int timeout = GWSApp::globalInstance()->getConfiguration().value( "timeout" ).toInt( 5 );
    QTimer::singleShot( timeout * 1000 , []{
        GWSApp::globalInstance()->exit( -1 );
    });

    // Start ticking
    QTimer::singleShot( 1000 , this , &GWSExecutionEnvironment::tick );
}

void GWSExecutionEnvironment::behave(){

    bool parallel_runnings = false;
    int total_entities = 0;
    int ready_entities = 0;
    int busy_entities = 0;
    int future_entities = 0;
    int ticked_entities = 0;
    qint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    qint64 min_tick = current_datetime;

    foreach( GWSExecutionEnvironmentElement* p , this->parallel_executions.values() ) {

        if( p->is_running ){
            parallel_runnings = true;
            if( p->isBusy() ){ continue; }
            total_entities += p->total_entities;
            ready_entities += p->ready_entities;
            busy_entities += p->busy_entities;
            future_entities += p->future_entities;
            ticked_entities += p->ticked_entities;
            //min_tick = (min_tick + p->min_tick) / 2;
            QTimer::singleShot( 0 , p , &GWSExecutionEnvironmentElement::behave );
        }
    }


    QString message = QString("Tick %1 , Entities : Ticked %2 | Ready %3 | Busy %4 | Future %5 | Total %6" )
                .arg( QDateTime::fromMSecsSinceEpoch( current_datetime ).toString("yyyy-MM-ddTHH:mm:ss") )
                .arg( ticked_entities , -8 )
                .arg( ready_entities , -8 )
                .arg( busy_entities , -8 )
                .arg( future_entities ,  -8 )
                .arg( total_entities , -8 );

        qInfo() << message;

        emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal(
                    QJsonObject({ { "message" , message } , { GWSTimeEnvironment::INTERNAL_TIME_PROP , (double)current_datetime } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );

    // Call again this function
    if( this->isRunning() && parallel_runnings ) {

        // DO NOT USE QTCONCURRENT FOR ANYTHING. IS SLOW AS HELL.
        // OTHERWISE IT WAITS FOR ALL PENDING ENTITIES TO TICK, GETS SLOW AND QUITE SYNCRHONOUS
        // DIRECTLY USE QTIMER::SINGLESHOT
        qint64 next_tick_in = qMax( 10.0 , 1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed() );

        // Store min tick
        if( min_tick < current_datetime && ticked_entities > 0 && min_tick > 0 ){
            GWSTimeEnvironment::globalInstance()->goBackToDatetime( min_tick );
            current_datetime = min_tick;
        }

        // WAIT SOME MORE
        if( min_tick <= 0 ){
            next_tick_in = 1000;
        }

        // DO NOT ADVANCE IN TIME
        if( ready_entities <= 0 ){
            GWSTimeEnvironment::globalInstance()->goBackMsecs( next_tick_in * GWSTimeEnvironment::globalInstance()->getTimeSpeed() );
        }

        QTimer::singleShot( next_tick_in , this , &GWSExecutionEnvironment::tick );
    }

}

void GWSExecutionEnvironment::stop(){

    if( !this->isRunning() ){ return; }

    this->setProperties( QJsonObject( {
        { ENTITY_RUNNING_PROP , false } ,
        { ENDED_SIMULATION_TIME , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() } ,
        { ENDED_REAL_TIME , QDateTime::currentMSecsSinceEpoch() }
    }) );

    emit this->stoppingExecutionSignal();
}

/**********************************************************************
 HELPER CLASS
**********************************************************************/

bool GWSExecutionEnvironment::GWSExecutionEnvironmentElement::isBusy() const{
    bool b;
    this->mutext.lockForRead();
    b = this->is_busy;
    this->mutext.unlock();
    return b;
}

void GWSExecutionEnvironment::GWSExecutionEnvironmentElement::behave(){

    this->mutext.lockForRead();
    if( this->is_busy ){
        this->mutext.unlock();
        return;
    }
    this->mutext.unlock();

    this->mutext.lockForWrite();
    this->is_busy = true;
    this->mutext.unlock();

    QList< int > entities_to_be_ticked_positions; // List of indexes of the currently_running_entities list
    this->ready_entities = 0;

    if( this->running_storage.isEmpty() ){

        QString message = QString("Execution has no more running entities" );

        qInfo() << message;

        emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal(
                    QJsonObject({ { "message" , message } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );

        this->is_running = false;
        return;
    }

    // Get current datetime in simulation
    qint64 executing_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();

    // Wait for entities that are delayed (if WAIT_FOR_ME).
    // Get min tick time and add some threshold to execute the entities that are more delayed.
    this->min_tick = -1;
    QString who_is_min_tick;
    QStringList what_is_min_tick_executing;
    this->ticked_entities = 0;
    this->busy_entities = 0;
    this->total_entities = 0;
    this->future_entities = 0;

    for( int i = 0 ; i < this->running_storage.size() ; i++ ){

        QSharedPointer<GWSEntity> entity = this->running_storage.at( i );

        if( entity.isNull() ){
            this->running_storage.removeAll( entity );
            continue;
        }

        this->total_entities++;
        QJsonObject properties_copy = entity->getProperties({ WAIT_FOR_ME_PROP , GWSTimeEnvironment::INTERNAL_TIME_PROP });

        if( entity->isBusy() && !properties_copy.value( WAIT_FOR_ME_PROP ).toBool( false ) ) {
            continue;
        }

        qint64 entity_time = properties_copy.value( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( executing_datetime );

        if( entity_time > 0 && min_tick <= 0 ){ min_tick = entity_time; }
        if( entity_time > 0 && min_tick > 0 ){ min_tick = qMin( min_tick , entity_time ); }
        if( min_tick == entity_time ){
            who_is_min_tick = entity->getUID();
            what_is_min_tick_executing = entity->getCurrentlyExecutingBehaviourUIDS();
        }

        // Shuffle list
        this->ready_entities++;
        entities_to_be_ticked_positions.insert( qrand() % this->ready_entities , i );
    }

    if( this->ready_entities > 0 ){

        if( min_tick <= 0 || min_tick > executing_datetime ){ min_tick = executing_datetime; }

        qint64 limit = min_tick + this->tick_time_window; // Add threshold, otherwise only the minest_tick entity is executed

        foreach( int i , entities_to_be_ticked_positions ){

            QSharedPointer<GWSEntity> entity = this->running_storage.at( i );

            if( !entity || entity->deleted ){
                this->running_storage.removeAll( entity );
                continue;
            }

            if( entity->isBusy() ){
                this->busy_entities++;
                continue;
            }

            QJsonObject properties_copy = entity->getProperties({ GWSTimeEnvironment::INTERNAL_TIME_PROP , GWSExecutionEnvironment::ENTITY_DEATH_PROP });

            qint64 entity_next_tick = properties_copy.value( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( -1 );

            if ( !properties_copy.value( GWSExecutionEnvironment::ENTITY_DEATH_PROP ).isNull() && min_tick >= properties_copy.value( GWSExecutionEnvironment::ENTITY_DEATH_PROP ).toDouble() ){
                GWSExecutionEnvironment::globalInstance()->unregisterEntity( entity );
                continue;
            }

            if( entity_next_tick > limit ){
                this->future_entities++;
                continue;
            }

            if( entity_next_tick <= 0 ){ entity->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , min_tick ); }

            // Call behave through tick for it to be executed in the entity's thread (important to avoid msec < 100)
            entity->incrementBusy(); // Increment here, Decrement after entity Tick()

            QTimer::singleShot( 0 , entity.data() , &GWSEntity::tick );

            if( this->max_entity_amount_per_tick > 0 && ++this->ticked_entities >= this->max_entity_amount_per_tick ){
                break;
            }
        }
    }

    this->mutext.lockForWrite();
    this->is_busy = false;
    this->mutext.unlock();
}
