#include <QJsonDocument>
#include <QJsonObject>

#include "../../app/App.h"
#include "TimeEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

QString geoworldsim::environment::TimeEnvironment::INTERNAL_TIME_PROP = "time";

geoworldsim::environment::TimeEnvironment* geoworldsim::environment::TimeEnvironment::globalInstance(){
    static TimeEnvironment instance;
    return &instance;
}

geoworldsim::environment::TimeEnvironment::TimeEnvironment() : Environment() ,
    software_started_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) ,
    simulation_datetime_msecs( QDateTime::currentMSecsSinceEpoch() ) {

    this->time_speed = App::globalInstance()->getConfiguration().value("speed").toDouble(1);
    this->simulation_datetime_msecs = App::globalInstance()->getConfiguration().value( "start" ).toDouble( QDateTime::currentMSecsSinceEpoch() );

    qInfo() << QString("TimeEnvironment created with speed %1 and datetime %2").arg( this->time_speed ).arg( QDateTime::fromMSecsSinceEpoch( this->simulation_datetime_msecs ).toString() );
    EnvironmentsGroup::globalInstance()->addEnvironment( this );
}

geoworldsim::environment::TimeEnvironment::~TimeEnvironment(){
}

/**********************************************************************
   IMPORTERS
**********************************************************************/

void geoworldsim::environment::TimeEnvironment::deserialize(QJsonObject json){
    if( json.contains("speed") ){
        this->setTimeSpeed( qMax( json["speed"].toObject()["value"].toDouble() , 0.1) );
    }
}

/**********************************************************************
   EXPORTERS
**********************************************************************/

QJsonObject geoworldsim::environment::TimeEnvironment::serialize(){
    QJsonObject json;
    json.insert( "datetime" , (qint64)this->getCurrentDateTime() );
    json.insert( "speed" , this->getTimeSpeed() );
    return json;
}

/**********************************************************************
   GETTERS
**********************************************************************/

qint64 geoworldsim::environment::TimeEnvironment::getCurrentDateTime() const{
    // Get elapsed time difference
    quint64 elapsed_msecs = ( QDateTime::currentMSecsSinceEpoch() - this->software_started_datetime_msecs ) * this->time_speed;
    return this->simulation_datetime_msecs + elapsed_msecs;
}

double geoworldsim::environment::TimeEnvironment::getTimeSpeed() const{
    return this->time_speed;
}

/*qint64 geoworldsim::environment::TimeEnvironment::getAgentInternalTime( QSharedPointer<Agent> agent ){
    return this->agent_internal_times.value( agent->getUID() , -1 );
}*/

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::environment::TimeEnvironment::setDatetime( quint64 datetime ){
    if( datetime >= 0 ){
        this->simulation_datetime_msecs = datetime;
        this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
    } else {
        qWarning() << QString("Invalid datetime : %1").arg( datetime );
    }
}

void geoworldsim::environment::TimeEnvironment::goBackToDatetime( quint64 datetime ){
    if( datetime >= 0 ){
        this->simulation_datetime_msecs = qMin( this->simulation_datetime_msecs , datetime );
        this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
    }
}

void geoworldsim::environment::TimeEnvironment::goBackMsecs( quint64 msecs ){
    this->simulation_datetime_msecs -= msecs;
    this->software_started_datetime_msecs = QDateTime::currentMSecsSinceEpoch();
}

void geoworldsim::environment::TimeEnvironment::setTimeSpeed( double time_speed ){
    this->time_speed = qMax(0.01 , time_speed); // Avoid time_speed = 0
}

/*void geoworldsim::environment::TimeEnvironment::setAgentInternalTime( QSharedPointer<Agent> agent , qint64 datetime){
    this->agent_internal_times.insert( agent->getUID() , datetime );
}*/

/*void geoworldsim::environment::TimeEnvironment::incrementAgentInternalTime( QSharedPointer<Agent> agent , TimeUnit seconds){
    QString agent_id = agent->getId();
    if( this->agent_internal_times.value( agent_id ) > 0 ){
        this->agent_internal_times.insert( agent_id , this->agent_internal_times.value( agent_id , 0 ) + qMax( 0.01 , seconds.number() ) * 1000 ); // Min 10 milliseconds
    }
}*/

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::environment::TimeEnvironment::registerEntity( QSharedPointer< Entity > agent) {

    Environment::registerEntity( agent );

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
    //this->connect( agent.data() , &Entity::entityPropertyChangedSignal , this , &geoworldsim::environment::TimeEnvironment::entityPropertyChanged );
}

void geoworldsim::environment::TimeEnvironment::unregisterEntity( QSharedPointer< Entity > agent){
    Environment::unregisterEntity( agent );

    //this->disconnect( agent.data() , &Entity::entityPropertyChangedSignal , this , &geoworldsim::environment::TimeEnvironment::entityPropertyChanged );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void geoworldsim::environment::TimeEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == INTERNAL_TIME_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        Entity* agent = dynamic_cast<Entity*>( object );
        if( !agent ){ return; }
        // Nothing to do
    }
}
