#include <QJsonDocument>
#include <QJsonObject>

#include "TimeEnvironment.h"

QString GWSTimeEnvironment::INTERNAL_TIME_PROP = "internal_time";

GWSTimeEnvironment* GWSTimeEnvironment::globalInstance(){
    static GWSTimeEnvironment instance;
    return &instance;
}

GWSTimeEnvironment::GWSTimeEnvironment() : GWSEnvironment() , started_datetime_msecs( QDateTime::currentDateTime().toMSecsSinceEpoch() ) {
    qInfo() << "TimeEnvironment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
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
    quint64 elapsed_msecs = ( QDateTime::currentMSecsSinceEpoch() - this->started_datetime_msecs ) * this->time_speed;
    return this->started_datetime_msecs + elapsed_msecs;
}

double GWSTimeEnvironment::getTimeSpeed() const{
    return this->time_speed;
}

qint64 GWSTimeEnvironment::getAgentInternalTime(const GWSAgent *agent) const{
    return this->getAgentInternalTime( agent->getId() );
}

qint64 GWSTimeEnvironment::getAgentInternalTime(QString agent_id) const{
    return this->agent_internal_times.value( agent_id , 0 );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSTimeEnvironment::setDatetime(quint64 datetime){
    if( datetime >= 0 ){
        this->started_datetime_msecs = datetime;
    } else {
        qWarning() << QString("Invalid datetime : %1").arg( datetime );
    }
}

void GWSTimeEnvironment::setTimeSpeed(double time_speed){
    this->time_speed = qMax(0.01 , time_speed); // Avoid time_speed = 0
}

void GWSTimeEnvironment::setAgentInternalTime(const GWSAgent *agent, const qint64 datetime){
    this->agent_internal_times.insert( agent->getId() , datetime );
}

void GWSTimeEnvironment::incrementAgentInternalTime(const GWSAgent* agent , GWSTimeUnit seconds){
    if( this->agent_internal_times.value( agent->getId() ) > 0 ){
        this->agent_internal_times.insert( agent->getId() , this->agent_internal_times.value( agent->getId() , 0 ) + qMax( 0.01 , seconds.number() ) * 1000 ); // Min 10 milliseconds
    }
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSTimeEnvironment::registerAgent(GWSAgent *agent, quint64 init_internal_time) {
    GWSEnvironment::registerAgent( agent );
    this->agent_internal_times.insert( agent->getId() , init_internal_time );
}

void GWSTimeEnvironment::unregisterAgent(GWSAgent *agent){
    GWSEnvironment::unregisterAgent( agent );
    this->agent_internal_times.remove( agent->getId() );
}
