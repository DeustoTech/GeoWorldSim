#include "Behaviour.h"

#include <QJsonValue>

#include "../../object/ObjectFactory.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"


QString geoworldsim::behaviour::Behaviour::BEHAVIOUR_DURATION = "duration";
QString geoworldsim::behaviour::Behaviour::SNAPSHOT_EVERY_TICKS = "snapshot_every";
QString geoworldsim::behaviour::Behaviour::SNAPSHOT_ATTRIBUTES = "snapshot_attributes";
QString geoworldsim::behaviour::Behaviour::SUB_BEHAVIOURS_PROP = "@sub_behaviours";
QString geoworldsim::behaviour::Behaviour::FINISH_CONDITION_PROP = "@finish_condition";
QString geoworldsim::behaviour::Behaviour::START_BEHAVIOUR_PROP = "start";

geoworldsim::behaviour::Behaviour::Behaviour() : Object(){
    this->setProperty( BEHAVIOUR_DURATION , 10 ); // 10 seconds by default
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void geoworldsim::behaviour::Behaviour::deserialize(const QJsonObject &json, QSharedPointer<Object> behaving_agent){
    this->behaving_entity = behaving_agent.dynamicCast< Entity >();
    Object::deserialize( json , behaving_agent );

    // SUBBEHAVIOURS
    if( json.keys().contains( SUB_BEHAVIOURS_PROP ) ){
        QJsonArray arr = json.value( SUB_BEHAVIOURS_PROP ).toArray();
        foreach(QJsonValue jb , arr ){
            QSharedPointer< Behaviour > behaviour = ObjectFactory::globalInstance()->fromJSON( jb.toObject() , this->getEntity() ).dynamicCast< Behaviour >();
            if( behaviour ){
                this->addSubbehaviour( behaviour );
            }
        }
    }

    // START BEHAVIOUR
    if( json.keys().contains( START_BEHAVIOUR_PROP ) && json.value( START_BEHAVIOUR_PROP ).toBool() ){
        this->getEntity()->addCurrentlyExecutingBehaviour( this->getSharedPointer().dynamicCast< Behaviour >() );
    }

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject geoworldsim::behaviour::Behaviour::serialize() const{
    QJsonObject json = Object::serialize();
    if( !this->sub_behaviours.isEmpty() ){
        QJsonArray arr;
        foreach( QSharedPointer< Behaviour > b , this->sub_behaviours ){
            arr.append( b->serialize() );
        }
        json.insert( SUB_BEHAVIOURS_PROP , arr );
    }
    /*if( !this->next_behaviour_ids.isEmpty() ){
        QJsonArray arr;
        foreach( QString id , this->next_behaviour_ids ){
            arr.append( id );
        }
        json.insert( NEXT_BEHAVIOURS_PROP , arr );
    }*/
    return json;
}

/***********************************************************08:35:00-17:10***********
 GETTERS
**********************************************************************/

QSharedPointer< geoworldsim::Entity > geoworldsim::behaviour::Behaviour::getEntity() const{
    return this->behaving_entity;
}

int geoworldsim::behaviour::Behaviour::getTickedAmount() const{
    return this->ticked_amount;
}

QList< QSharedPointer<geoworldsim::behaviour::Behaviour> > geoworldsim::behaviour::Behaviour::getSubs() const{
    return this->sub_behaviours;
}

QJsonValue geoworldsim::behaviour::Behaviour::getProperty( const QString& name ) const{
   return ObjectFactory::simpleOrParentPropertyName( name , this->getSharedPointer() , this->getEntity() );
}

void geoworldsim::behaviour::Behaviour::setProperty( const QString& name , const QJsonValue &value ){

    if( name.startsWith("<") && name.endsWith(">") ){
        QSharedPointer< Entity > agent = this->getEntity();
        QString str = name;
        str = str.remove( 0 , 1 );
        str = str.remove( str.length() - 1 , 1 );
        agent->setProperty( str , value );
    } else {
        Object::setProperty( name , value );
    }
}

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::behaviour::Behaviour::addSubbehaviour(QSharedPointer<geoworldsim::behaviour::Behaviour> sub_behaviour){
    this->sub_behaviours.append( sub_behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the GWSAgent for behave() to be executed in the agents thread.
 **/
QPair< double , QJsonArray > geoworldsim::behaviour::Behaviour::tick( qint64 behaviour_ticked_time ){

    QSharedPointer< Entity > entity = this->getEntity();
    //qDebug() << QString("Agent %1 %2 executing behaviour %3 %4").arg( this->getAgent()->metaObject()->className() ).arg( this->getAgent()->getId() ).arg( this->metaObject()->className() ).arg( this->getId() );

    QPair< double , QJsonArray > nexts;
    this->behaving_time = behaviour_ticked_time;
    this->ticked_amount++;

    entity->incrementBusy();
    nexts = this->behave();
    entity->decrementBusy();

    // Check if need to send snapshot
    quint64 snapshots = this->getProperty( SNAPSHOT_EVERY_TICKS ).toDouble( 0 );
    if( snapshots > 0 && this->ticked_amount % snapshots == 0 ){
        this->snapshotEntity( entity );
    }

    return nexts;
}

QPair<double, QJsonArray> geoworldsim::behaviour::Behaviour::behave(){

    QJsonArray nexts;
    double max_duration = 0;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach( QSharedPointer< Behaviour > sub, this->sub_behaviours) {
        QPair< double , QJsonArray > n = sub->tick( this->behaving_time );
        foreach ( QJsonValue v , n.second ) {
            nexts.append( v );
        }
        max_duration = qMax( max_duration , n.first );
    }

    return QPair< double , QJsonArray>( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , nexts );
}


void geoworldsim::behaviour::Behaviour::snapshotEntity( QSharedPointer<Entity> entity ) const {

    // If SNAPSHOT_ATTRIBUTES has some values
    QJsonArray snapshot_attributes = this->getProperty( SNAPSHOT_ATTRIBUTES ).toArray();
    if( snapshot_attributes.isEmpty() ){

        // Send entity unfiltered
        emit geoworldsim::environment::CommunicationEnvironment::globalInstance()->sendEntitySignal( entity->serialize() );

    } else {

        QJsonObject filtered_json;
        QJsonObject serialized_entity = entity->serialize();

        // Mandatory attributes
        snapshot_attributes.append( geoworldsim::Object::GWS_UID_PROP );
        snapshot_attributes.append( "type" );
        snapshot_attributes.append( environment::TimeEnvironment::INTERNAL_TIME_PROP );

        foreach( QJsonValue attr , snapshot_attributes ) {
            filtered_json.insert( attr.toString() , serialized_entity.value( attr.toString() ) );
        }

        // Send entity unfiltered
        emit geoworldsim::environment::CommunicationEnvironment::globalInstance()->sendEntitySignal( entity->serialize() );
    }

}
