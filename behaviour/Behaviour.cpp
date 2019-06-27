#include "Behaviour.h"

#include <QJsonValue>

#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSBehaviour::BEHAVIOUR_DURATION = "duration";
QString GWSBehaviour::SUB_BEHAVIOURS_PROP = "@sub_behaviours";
QString GWSBehaviour::FINISH_CONDITION_PROP = "@finish_condition";
QString GWSBehaviour::START_BEHAVIOUR_PROP = "start";

GWSBehaviour::GWSBehaviour() : GWSObject(){
    this->setProperty( BEHAVIOUR_DURATION , 10 ); // 10 seconds
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSBehaviour::deserialize(const QJsonObject &json, QSharedPointer<GWSObject> behaving_agent){
    this->behaving_entity = behaving_agent.dynamicCast<GWSEntity>();
    GWSObject::deserialize( json , behaving_agent );

    // SUBBEHAVIOURS
    if( json.keys().contains( SUB_BEHAVIOURS_PROP ) ){
        QJsonArray arr = json.value( SUB_BEHAVIOURS_PROP ).toArray();
        foreach(QJsonValue jb , arr ){
            QSharedPointer<GWSBehaviour> behaviour = GWSObjectFactory::globalInstance()->fromJSON( jb.toObject() , this->getEntity() ).dynamicCast<GWSBehaviour>();
            if( behaviour ){
                this->addSubbehaviour( behaviour );
            }
        }
    }

    // START BEHAVIOUR
    if( json.keys().contains( START_BEHAVIOUR_PROP ) && json.value( START_BEHAVIOUR_PROP ).toBool() ){
        this->getEntity()->addCurrentlyExecutingBehaviour( this->getSharedPointer().dynamicCast<GWSBehaviour>() );
    }

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSBehaviour::serialize() const{
    QJsonObject json = GWSObject::serialize();
    if( !this->sub_behaviours.isEmpty() ){
        QJsonArray arr;
        foreach( QSharedPointer<GWSBehaviour> b , this->sub_behaviours ){
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

QSharedPointer<GWSEntity> GWSBehaviour::getEntity() const{
    return this->behaving_entity;
}

QList< QSharedPointer<GWSBehaviour> > GWSBehaviour::getSubs() const{
    return this->sub_behaviours;
}

QJsonValue GWSBehaviour::getProperty( const QString& name ) const{
   return GWSObjectFactory::simpleOrParentPropertyName( name , this->getSharedPointer() , this->getEntity() );
}

void GWSBehaviour::setProperty( const QString& name , const QJsonValue &value ){

    if( name.startsWith("<") && name.endsWith(">") ){
        QSharedPointer<GWSEntity> agent = this->getEntity();
        QString str = name;
        str = str.remove( 0 , 1 );
        str = str.remove( str.length() - 1 , 1 );
        agent->setProperty( str , value );
    } else {
        GWSObject::setProperty( name , value );
    }
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSBehaviour::addSubbehaviour(QSharedPointer<GWSBehaviour> sub_behaviour){
    this->sub_behaviours.append( sub_behaviour );
}

/*void GWSBehaviour::addNextBehaviour( QSharedPointer<GWSBehaviour> next_behaviour){
    this->next_behaviour_ids.append( next_behaviour->getId() );
}*/

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the GWSAgent for behave() to be executed in the agents thread.
 **/
QPair< double , QJsonArray > GWSBehaviour::tick( qint64 behaviour_ticked_time ){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    //qDebug() << QString("Agent %1 %2 executing behaviour %3 %4").arg( this->getAgent()->metaObject()->className() ).arg( this->getAgent()->getId() ).arg( this->metaObject()->className() ).arg( this->getId() );

    QPair< double , QJsonArray > nexts;
    this->behaving_time = behaviour_ticked_time;

    agent->incrementBusy();
    nexts = this->behave();
    agent->decrementBusy();

    // Calculate how much to increment agent internal time
    qint64 increment_time = qMax( 100.0 , this->getProperty( BEHAVIOUR_DURATION ).toDouble() * 1000 ); // At least 0.1 seconds
    qint64 agent_current_time = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    if( agent_current_time < 0 ){
        agent_current_time = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
    }

    // Compare how much has been spent or if some other behaviour incremented the time
    qint64 max_time = qMax( (qint64)(behaviour_ticked_time + increment_time) , agent_current_time );
    agent->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , max_time );
    return nexts;
}

QPair<double, QJsonArray> GWSBehaviour::behave(){

    QJsonArray nexts;
    double max_duration = 0;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach( QSharedPointer<GWSBehaviour> sub, this->sub_behaviours) {
        QPair< double , QJsonArray > n = sub->tick( this->behaving_time );
        foreach ( QJsonValue v , n.second ) {
            nexts.append( v );
        }
        max_duration = qMax( max_duration , n.first );
    }

    return QPair< double , QJsonArray>( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , nexts );
}
