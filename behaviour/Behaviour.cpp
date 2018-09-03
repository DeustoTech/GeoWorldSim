#include "Behaviour.h"

#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSBehaviour::INCREMENT_AGENT_TIME_PROP = "duration";
QString GWSBehaviour::SUB_BEHAVIOURS_PROP = "@sub_behaviours";
QString GWSBehaviour::FINISH_CONDITION_PROP = "@finish_condition";
QString GWSBehaviour::NEXT_BEHAVIOURS_PROP = "@next";
QString GWSBehaviour::START_BEHAVIOUR_PROP = "start";

GWSBehaviour::GWSBehaviour() : GWSObject(){
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
    if( json.keys().contains( NEXT_BEHAVIOURS_PROP ) ){
        QJsonArray next_ids = json.value( NEXT_BEHAVIOURS_PROP ).toArray();
        foreach( QJsonValue id , next_ids ){
            // Find next behaviour in agent
            QSharedPointer<GWSBehaviour> next_behaviour = this->getAgent()->getBehaviour( id.toString() );
            if( next_behaviour ){
                this->addNextBehaviour( next_behaviour );
            } else {
                qDebug() << QString("Behaviour %1 %2 tried to find next %3 but does not exist. Please define in the JSON first the referenced Behaviour.")
                            .arg( this->metaObject()->className() )
                            .arg( this->getId() )
                            .arg( id.toString() );
            }
        }
    }

    // START BEHAVIOUR
    if( json.keys().contains( START_BEHAVIOUR_PROP ) && json.value( START_BEHAVIOUR_PROP ).toBool() ){
        this->getAgent()->addStartBehaviour( this->getSharedPointer().dynamicCast<GWSBehaviour>() );
    }

}

/**********************************************************************
 EXPOERTERS
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
    if( !this->next_behaviours.isEmpty() ){
        QJsonArray arr;
        foreach( QSharedPointer<GWSBehaviour>b , this->next_behaviours ){
            arr.append( b->getId() );
        }
        json.insert( NEXT_BEHAVIOURS_PROP , arr );
    }
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

QList< QSharedPointer<GWSBehaviour> > GWSBehaviour::getNext(){
    return this->next_behaviours;
}

bool GWSBehaviour::finished(){

    int condition = this->getProperty( FINISH_CONDITION_PROP ).toInt();
    if( condition <= 0 ){ condition = this->sub_behaviours.size(); }
    int finished_amount = 0;

    foreach (QSharedPointer<GWSBehaviour> sub, this->sub_behaviours){
        finished_amount += sub->finished() ? 1 : 0;
    }
    return finished_amount >= condition;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSBehaviour::addSubbehaviour(QSharedPointer<GWSBehaviour> sub_behaviour){
    this->sub_behaviours.append( sub_behaviour );
}

void GWSBehaviour::addNextBehaviour( QSharedPointer<GWSBehaviour> next_behaviour){
    this->next_behaviours.append( next_behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the GWSAgent for behave() to be executed in the agents thread.
 **/
bool GWSBehaviour::tick( qint64 behaviour_ticked_time ){

    qDebug() << QString("Agent %1 %2 executing behaviour %3 %4").arg( this->getAgent()->metaObject()->className() ).arg( this->getAgent()->getId() ).arg( this->metaObject()->className() ).arg( this->getId() );

    bool behaved_correctly = false;
    this->behaving_time = behaviour_ticked_time;

    this->getAgent()->incrementBusy();
    behaved_correctly = this->behave();
    this->getAgent()->decrementBusy();

    // Calculate how much to increment agent internal time
    qint64 increment_time = qMax( 100 , this->getProperty( INCREMENT_AGENT_TIME_PROP ).toInt() ); // At least 0.1 seconds
    qint64 agent_current_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getAgent() );

    // Compare how much has been spent or if some other behaviour incremented the time
    qint64 max_time = qMax( (qint64)(behaviour_ticked_time + increment_time) , agent_current_time );
    GWSTimeEnvironment::globalInstance()->setAgentInternalTime( this->getAgent() , max_time );

    return behaved_correctly;
}

bool GWSBehaviour::behave(){

    bool success = true;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach( QSharedPointer<GWSBehaviour> sub, this->sub_behaviours) {

        if( !sub->finished() ){
            success = sub->tick( this->behaving_time );
            if( !success ){
                break;
            }
        }
    }

    return success;
}
