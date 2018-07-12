#include <QJsonDocument>
#include <QJsonObject>

#include "TimeEnvironment.h"

#include "../../util/conversors/custom_json/CustomJsonConversor.h"

GWSTimeEnvironment* GWSTimeEnvironment::globalInstance(){
    static GWSTimeEnvironment instance;
    return &instance;
}

GWSTimeEnvironment::GWSTimeEnvironment() : GWSEnvironment() , current_datetime_msecs( QDateTime::currentDateTime().toMSecsSinceEpoch() ) {
    qInfo() << "TimeEnvironment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSTimeEnvironment::~GWSTimeEnvironment(){
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

void GWSTimeEnvironment::deserialize(QJsonObject json){
    if( json.contains("speed") ){
        this->setTimeSpeed( qMax( json["speed"].toObject()["value"].toDouble() , 0.1) );
    }
}

/**********************************************************************
   GETTERS
**********************************************************************/

qint64 GWSTimeEnvironment::getCurrentDateTime() const{
    return this->current_datetime_msecs;
}

double GWSTimeEnvironment::getTimeSpeed() const{
    return this->time_speed;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSTimeEnvironment::setDate( QDate& date ){
    if( date.isValid() ){
        QDateTime temp = QDateTime::fromMSecsSinceEpoch( this->current_datetime_msecs );
        temp.setDate( date );
        this->current_datetime_msecs = temp.toMSecsSinceEpoch();
    } else {
        qWarning() << QString("Invalid date : %1").arg( date.toString() );
    }
}

void GWSTimeEnvironment::setTime( QTime& time ){
    if( time.isValid() ){
        QDateTime temp = QDateTime::fromMSecsSinceEpoch( this->current_datetime_msecs );
        temp.setTime( time );
        this->current_datetime_msecs = temp.toMSecsSinceEpoch();
    } else {
        qWarning() << QString("Invalid time : %1").arg( time.toString() );
    }
}

void GWSTimeEnvironment::setDatetime(quint64 datetime){
    if( datetime > 0 ){
        this->current_datetime_msecs = datetime;
    } else {
        qWarning() << QString("Invalid datetime : %1").arg( datetime );
    }
}

void GWSTimeEnvironment::setTimeSpeed(double time_speed){
    this->time_speed = qMax(0.01 , time_speed); // Avoid time_speed = 0
}

