#include <QJsonDocument>
#include <QJsonObject>

#include "../../app/App.h"
#include "TimeEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

QString GWSTimeEnvironment::INTERNAL_TIME_PROP = "time";

GWSTimeEnvironment* GWSTimeEnvironment::globalInstance(){
    static GWSTimeEnvironment instance;
    return &instance;
}

GWSTimeEnvironment::GWSTimeEnvironment() : GWSEnvironment() ,
    software_started_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) ,
    simulation_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) {
    this->time_speed = GWSApp::globalInstance()->getConfiguration().value("speed").toDouble(1);
    this->simulation_datetime_msecs = GWSApp::globalInstance()->getConfiguration().value( "start" ).toDouble( QDateTime::currentMSecsSinceEpoch() );
    qInfo() << QString("TimeEnvironment created with speed %1 and datetime %2").arg( this->time_speed ).arg( QDateTime::fromMSecsSinceEpoch( this->simulation_datetime_msecs ).toString() );
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSTimeEnvironment::~GWSTimeEnvironment(){
}

/**********************************************************************
   IMPORTERS
**********************************************************************/

void GWSTimeEnvironment::deserialize(QJsonObject json){
    if( json.contains("speed") ){
        this->setTimeSpeed( qMax( json["speed"].toObject()["value"].toDouble() , 0.1) );
    }
}

/**********************************************************************
   EXPORTERS
**********************************************************************/

QJsonObject GWSTimeEnvironment::serialize(){
    QJsonObject json;
    json.insert( "datetime" , (qint64)this->getCurrentDateTime() );
    json.insert( "speed" , this->getTimeSpeed() );
    return json;
}

/**********************************************************************
   GETTERS
**********************************************************************/

qint64 GWSTimeEnvironment::getCurrentDateTime() const{
    // Get elapsed time difference
    quint64 elapsed_msecs = ( QDateTime::currentMSecsSinceEpoch() - this->software_started_datetime_msecs ) * this->time_speed;
    return this->simulation_datetime_msecs + elapsed_msecs;
}

double GWSTimeEnvironment::getTimeSpeed() const{
    return this->time_speed;
}

/*qint64 GWSTimeEnvironment::getAgentInternalTime( QSharedPointer<GWSAgent> agent ){
    return this->agent_internal_times.value( agent->getUID() , -1 );
}*/

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSTimeEnvironment::setDatetime(quint64 datetime){
    if( datetime >= 0 ){
        this->simulation_datetime_msecs = datetime;
        this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
    } else {
        qWarning() << QString("Invalid datetime : %1").arg( datetime );
    }
}

void GWSTimeEnvironment::goBackToDatetime(quint64 datetime){
    if( datetime >= 0 ){
        this->simulation_datetime_msecs = qMin( this->simulation_datetime_msecs , datetime );
        this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
    }
}

void GWSTimeEnvironment::goBackMsecs(quint64 msecs){
    this->simulation_datetime_msecs -= msecs;
    this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
}

void GWSTimeEnvironment::setTimeSpeed(double time_speed){
    this->time_speed = qMax(0.01 , time_speed); // Avoid time_speed = 0
}

/*void GWSTimeEnvironment::setAgentInternalTime( QSharedPointer<GWSAgent> agent , qint64 datetime){
    this->agent_internal_times.insert( agent->getUID() , datetime );
}*/

/*void GWSTimeEnvironment::incrementAgentInternalTime( QSharedPointer<GWSAgent> agent , GWSTimeUnit seconds){
    QString agent_id = agent->getId();
    if( this->agent_internal_times.value( agent_id ) > 0 ){
        this->agent_internal_times.insert( agent_id , this->agent_internal_times.value( agent_id , 0 ) + qMax( 0.01 , seconds.number() ) * 1000 ); // Min 10 milliseconds
    }
}*/

/**********************************************************************
 METHODS
**********************************************************************/

void GWSTimeEnvironment::registerEntity( QSharedPointer<GWSEntity> agent) {

    GWSEnvironment::registerEntity( agent );

    // INTERNAL TIME
    if( !agent->getProperty( INTERNAL_TIME_PROP ).isNull() ){

        qint64 init_internal_time = this->getCurrentDateTime();
        QJsonValue internal_time = agent->getProperty( INTERNAL_TIME_PROP );

        if( internal_time.isDouble() ){
            init_internal_time = internal_time.toDouble( init_internal_time );
        } else {
            init_internal_time = QDateTime::fromString( internal_time.toString() ).toMSecsSinceEpoch();
        }

        agent->setProperty( INTERNAL_TIME_PROP , (double)init_internal_time );
    }

    if( agent->getProperty( SKIP_INDEXING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    //this->connect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &GWSTimeEnvironment::entityPropertyChanged );
}

void GWSTimeEnvironment::unregisterEntity( QSharedPointer<GWSEntity> agent){
    GWSEnvironment::unregisterEntity( agent );

    //this->disconnect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &GWSTimeEnvironment::entityPropertyChanged );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSTimeEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == INTERNAL_TIME_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        GWSEntity* agent = dynamic_cast<GWSEntity*>( object );
        if( !agent ){ return; }
        // Nothing to do
    }
}
