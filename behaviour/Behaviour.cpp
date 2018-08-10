#include "Behaviour.h"

#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSBehaviour::INCREMENT_AGENT_TIME_PROP = "@forward_time";
QString GWSBehaviour::SUB_BEHAVIOURS_PROP = "@sub_behaviours";
QString GWSBehaviour::SUB_BEHAVIOURS_CONDITION_PROP = "@sub_behaviours_condition";
QString GWSBehaviour::NEXT_BEHAVIOURS_PROP = "@next";
QString GWSBehaviour::START_BEHAVIOUR_PROP = "start";

GWSBehaviour::GWSBehaviour(GWSAgent* behaving_agent ) : GWSObject( behaving_agent ){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSBehaviour::deserialize(QJsonObject json){
    GWSObject::deserialize( json );

    // SUBBEHAVIOURS
    if( json.keys().contains( SUB_BEHAVIOURS_PROP ) ){
        QJsonArray arr = json.value( SUB_BEHAVIOURS_PROP ).toArray();
        foreach(QJsonValue jb , arr ){
            GWSBehaviour* behaviour = dynamic_cast<GWSBehaviour*>( GWSObjectFactory::globalInstance()->fromJSON( jb.toObject() , this->getAgent() ) );
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
            GWSBehaviour* next_behaviour = this->getAgent()->getBehaviour( id.toString() );
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
        this->getAgent()->setStartBehaviour( this );
    }

}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

QJsonObject GWSBehaviour::serialize() const{
    QJsonObject json = GWSObject::serialize();
    if( !this->sub_behaviours.isEmpty() ){
        QJsonArray arr;
        foreach( GWSBehaviour* b , this->sub_behaviours ){
            arr.append( b->serialize() );
        }
        json.insert( SUB_BEHAVIOURS_PROP , arr );
    }
    if( !this->next_behaviours.isEmpty() ){
        QJsonArray arr;
        foreach( GWSBehaviour* b , this->next_behaviours ){
            arr.append( b->getId() );
        }
        json.insert( NEXT_BEHAVIOURS_PROP , arr );
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSAgent* GWSBehaviour::getAgent(){
    return dynamic_cast<GWSAgent*>( this->parent() );
}

quint64 GWSBehaviour::getBehavingTime() const {
    return this->behaving_time;
}

QList<GWSBehaviour*> GWSBehaviour::getNext(){
    return this->next_behaviours;
}

bool GWSBehaviour::finished(){

    // SUBBEHAVIOURS can act as:
    // AND, all must be finished to finish the behaviour
    // OR, if any has finished, the behaviour has finished
    // by DEFAULT they will be OR
    QString condition = this->getProperty( SUB_BEHAVIOURS_CONDITION_PROP ).toString();
    if( condition.isEmpty() ){ condition = "or"; }
    bool finished = condition == "or" ? false : true ;

    foreach (GWSBehaviour* sub, this->sub_behaviours){

        if( condition == "or" ){
            finished = finished || sub->finished();
        }
        if( condition == "and" ){
            finished = finished && sub->finished();
        }
    }
    return finished;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSBehaviour::addSubbehaviour(GWSBehaviour *sub_behaviour){
    this->sub_behaviours.append( sub_behaviour );
}

void GWSBehaviour::addNextBehaviour(GWSBehaviour *next_behaviour){
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
    qint64 max_time = qMax( (qint64)(this->behaving_time + increment_time) , agent_current_time );
    GWSTimeEnvironment::globalInstance()->setAgentInternalTime( this->getAgent() , max_time );

    return behaved_correctly;
}

bool GWSBehaviour::behave(){

    bool success = true;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach(GWSBehaviour* sub, this->sub_behaviours) {

        if( !sub->finished() ){
            success = sub->tick( this->getBehavingTime() );
            if( !success ){
                break;
            }
        }
    }

    return success;
}
