#include "Behaviour.h"

GWSBehaviour::GWSBehaviour(GWSAgent* behaving_agent ) : GWSObject( behaving_agent ){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSBehaviour::deserialize(QJsonObject json){
    GWSObject::deserialize( json );
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

QJsonObject GWSBehaviour::serialize() const{
    return GWSObject::serialize();
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSAgent* GWSBehaviour::getAgent(){
    return dynamic_cast<GWSAgent*>( this->parent() );
}

GWSBehaviour* GWSBehaviour::getNext(){
    return this->next_behaviour;
}

bool GWSBehaviour::finished(){
    bool finished = true;
    foreach (GWSBehaviour* sub, this->sub_behaviours){
        finished &= sub->finished();
    }
    return finished;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSBehaviour::addSubbehaviour(GWSBehaviour *sub_behaviour){
    Q_ASSERT( this->parent() == sub_behaviour->parent() );
    this->sub_behaviours.append( sub_behaviour );
}

void GWSBehaviour::setNextBehaviour(GWSBehaviour *next_behaviour){
    Q_ASSERT( this->parent() == next_behaviour->parent() );
    this->next_behaviour = next_behaviour;
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the GWSAgent for behave() to be executed in the agents thread.
 **/
bool GWSBehaviour::tick(){

    bool behaved_correctly = false;

    this->getAgent()->incrementBusy();

    behaved_correctly = this->behave();

    this->getAgent()->decrementBusy();

    return behaved_correctly;
}

bool GWSBehaviour::behave(){

    bool success = true;

    // A parent behaviour will iterate all its child behaviours at each behave call
    foreach(GWSBehaviour* sub, this->sub_behaviours) {

        if( !sub->finished() ){
            success = sub->tick();
            if( !success ){
                break;
            }
        }
    }

    return success;
}
