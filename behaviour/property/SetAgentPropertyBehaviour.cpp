#include "SetAgentPropertyBehaviour.h"

QString SetAgentPropertyBehaviour::PROPERTY_NAME = "property_name";
QString SetAgentPropertyBehaviour::PROPERTY_VALUE = "property_value";

SetAgentPropertyBehaviour::SetAgentPropertyBehaviour() : GWSBehaviour(){

}


bool SetAgentPropertyBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Unless the value of the property equals the behaviour's input value
    if ( this->getProperty( PROPERTY_VALUE ) != agent->getProperty( this->getProperty( PROPERTY_NAME ).toString() ) ){
        return false;
    }

    return true;
}


bool SetAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty(  this->getProperty( PROPERTY_NAME ).toString() , this->getProperty( PROPERTY_VALUE )  );
    return true;

}
