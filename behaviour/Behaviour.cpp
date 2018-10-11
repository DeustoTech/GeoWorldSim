#include "Behaviour.h"

#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSBehaviour::BEHAVIOUR_DURATION = "duration";
QString GWSBehaviour::SUB_BEHAVIOURS_PROP = "@sub_behaviours";
QString GWSBehaviour::FINISH_CONDITION_PROP = "@finish_condition";
QString GWSBehaviour::START_BEHAVIOUR_PROP = "start";

GWSBehaviour::GWSBehaviour() : GWSObject(){
    this->setProperty( BEHAVIOUR_DURATION , 1000 );
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSBehaviour::deserialize(QJsonObject json, QSharedPointer<GWSObject> behaving_agent){
    this->behaving_agent = behaving_agent.dynamicCast<GWSAgent>();
    GWSObject::deserialize( json , behaving_agent );

    // SUBBEHAVIOURS
    if( json.keys().contains( SUB_BEHAVIOURS_PROP ) ){
        QJsonArray arr = json.value( SUB_BEHAVIOURS_PROP ).toArray();
        foreach(QJsonValue jb , arr ){
            QSharedPointer<GWSBehaviour> behaviour = GWSObjectFactory::globalInstance()->fromJSON( jb.toObject() , this->getAgent() ).dynamicCast<GWSBehaviour>();
            if( behaviour ){
                this->addSubbehaviour( behaviour );
            }
        }
    }

    // NEXT BEHAVIOURS
    /*if( json.keys().contains( NEXT_BEHAVIOURS_PROP ) ){
        QJsonArray next_ids = json.value( NEXT_BEHAVIOURS_PROP ).toArray();
        if( next_ids.isEmpty() ){ next_ids.append( json.value( NEXT_BEHAVIOURS_PROP ).toString() ); }
        foreach( QJsonValue id , next_ids ){
            this->next_behaviour_ids.append( id.toString() );
        }
    }*/

    // START BEHAVIOUR
    if( json.keys().contains( START_BEHAVIOUR_PROP ) && json.value( START_BEHAVIOUR_PROP ).toBool() ){
        this->getAgent()->addCurrentlyExecutingBehaviour( this->getSharedPointer().dynamicCast<GWSBehaviour>() );
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

/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<GWSAgent> GWSBehaviour::getAgent(){
    return this->behaving_agent;
}

QList< QSharedPointer<GWSBehaviour> > GWSBehaviour::getSubs(){
    return this->sub_behaviours;
}

/*QList< QSharedPointer<GWSBehaviour> > GWSBehaviour::getNexts(){
    QList< QSharedPointer<GWSBehaviour> > nexts;
    foreach(QString id , this->next_behaviour_ids ){
        QSharedPointer<GWSBehaviour> b = this->getAgent()->getBehaviour( id );
        if( b ){
            nexts.append( b );
        }
    }
    return nexts;
}*/

/*bool GWSBehaviour::canContinueToNext(){

    int condition = this->getProperty( FINISH_CONDITION_PROP ).toInt();
    if( condition <= 0 ){ condition = this->sub_behaviours.size(); }
    int finished_amount = 0;

    foreach (QSharedPointer<GWSBehaviour> sub, this->sub_behaviours){
        finished_amount += sub->canContinueToNext() ? 1 : 0;
    }
    return finished_amount >= condition;
}*/

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
QStringList GWSBehaviour::tick( qint64 behaviour_ticked_time ){

    //qDebug() << QString("Agent %1 %2 executing behaviour %3 %4").arg( this->getAgent()->metaObject()->className() ).arg( this->getAgent()->getId() ).arg( this->metaObject()->className() ).arg( this->getId() );

    QStringList nexts;
    this->behaving_time = behaviour_ticked_time;

    this->getAgent()->incrementBusy();
    nexts = this->behave();
    this->getAgent()->decrementBusy();

    // Calculate how much to increment agent internal time
    qint64 increment_time = qMax( 100.0 , this->getProperty( BEHAVIOUR_DURATION ).toDouble() * 1000 ); // At least 0.1 seconds
    qint64 agent_current_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getAgent() );

    // Compare how much has been spent or if some other behaviour incremented the time
    qint64 max_time = qMax( (qint64)(behaviour_ticked_time + increment_time) , agent_current_time );
    GWSTimeEnvironment::globalInstance()->setAgentInternalTime( this->getAgent() , max_time );
    return nexts;
}

QStringList GWSBehaviour::behave(){

    QStringList nexts;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach( QSharedPointer<GWSBehaviour> sub, this->sub_behaviours) {
        nexts.append( sub->tick( this->behaving_time ) );
    }

    return nexts;
}
