#include "SetAgentPropertyBehaviour.h"

QString SetAgentPropertyBehaviour::PROPERTY_NAME = "property_name";
QString SetAgentPropertyBehaviour::PROPERTY_VALUE = "property_value";
QString SetAgentPropertyBehaviour::NEXTS = "nexts";

SetAgentPropertyBehaviour::SetAgentPropertyBehaviour() : GWSBehaviour(){

}


QJsonArray SetAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString property_name = this->getProperty( PROPERTY_NAME ).toString();
    QJsonValue property_value = this->getProperty( PROPERTY_VALUE );

    agent->setProperty( property_name , property_value );
    return this->getProperty( NEXTS ).toArray();

}
