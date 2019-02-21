#include <QJsonDocument>
#include <QJsonObject>

#include "TimeEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

QString GWSTimeEnvironment::INTERNAL_TIME_PROP = "time";
QString GWSTimeEnvironment::WAIT_FOR_ME_PROP = "wait_for_me";

GWSTimeEnvironment* GWSTimeEnvironment::globalInstance(){
    static GWSTimeEnvironment instance;
    return &instance;
}

GWSTimeEnvironment::GWSTimeEnvironment() : GWSEnvironment() , software_started_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) , simulation_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) {
    qInfo() << "TimeEnvironment created";
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

qint64 GWSTimeEnvironment::getAgentInternalTime( QSharedPointer<GWSAgent> agent ){
    return this->agent_internal_times.value( agent->getId() , this->getCurrentDateTime() );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSTimeEnvironment::setDatetime(quint64 datetime){
    if( datetime >= 0 ){
        this->simulation_datetime_msecs = datetime;
    } else {
        qWarning() << QString("Invalid datetime : %1").arg( datetime );
    }
}

void GWSTimeEnvironment::setTimeSpeed(double time_speed){
    this->time_speed = qMax(0.01 , time_speed); // Avoid time_speed = 0
}

void GWSTimeEnvironment::setAgentInternalTime( QSharedPointer<GWSAgent> agent , qint64 datetime){
    this->agent_internal_times.insert( agent->getId() , datetime );
}

/*void GWSTimeEnvironment::incrementAgentInternalTime( QSharedPointer<GWSAgent> agent , GWSTimeUnit seconds){
    QString agent_id = agent->getId();
    if( this->agent_internal_times.value( agent_id ) > 0 ){
        this->agent_internal_times.insert( agent_id , this->agent_internal_times.value( agent_id , 0 ) + qMax( 0.01 , seconds.number() ) * 1000 ); // Min 10 milliseconds
    }
}*/

/**********************************************************************
 METHODS
**********************************************************************/

void GWSTimeEnvironment::registerAgent( QSharedPointer<GWSAgent> agent) {
    GWSEnvironment::registerAgent( agent );

    // INTERNAL TIME
    quint64 init_internal_time = agent->getProperty( INTERNAL_TIME_PROP ).toInt();
    this->agent_internal_times.insert( agent->getId() , init_internal_time );
}

void GWSTimeEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent){
    GWSEnvironment::unregisterAgent( agent );
    this->agent_internal_times.remove( agent->getId() );
}
