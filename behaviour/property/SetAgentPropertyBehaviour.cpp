#include "SetAgentPropertyBehaviour.h"

QString SetAgentPropertyBehaviour::PROPERTY_NAME = "property_name";
QString SetAgentPropertyBehaviour::PROPERTY_VALUE = "property_value";

SetAgentPropertyBehaviour::SetAgentPropertyBehaviour() : GWSBehaviour(){

}


bool SetAgentPropertyBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Unless the value of the property equals the behaviour's input value
    if ( agent->getProperty( PROPERTY_NAME ).toString() != ( SetAgentPropertyBehaviour::PROPERTY_VALUE ) ){
        return false;
    }

    return true;
}


bool SetAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty(  PROPERTY_NAME , PROPERTY_VALUE  );
    return true;

}
