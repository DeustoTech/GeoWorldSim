#include "SetAgentPropertyBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

QString SetAgentPropertyBehaviour::AGENT_ID = "agent_to_set_id";
QString SetAgentPropertyBehaviour::PROPERTY_NAME = "property_name";
QString SetAgentPropertyBehaviour::PROPERTY_VALUE = "property_value";
QString SetAgentPropertyBehaviour::NEXTS = "nexts";

SetAgentPropertyBehaviour::SetAgentPropertyBehaviour() : GWSBehaviour(){

}


QJsonArray SetAgentPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( AGENT_ID ).toString();
    QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getByUID( agent_id );
    if( !agent ){
        return this->getProperty( NEXTS ).toArray();
    }

    QString property_name = this->getProperty( PROPERTY_NAME ).toString();
    QJsonValue property_value = this->getProperty( PROPERTY_VALUE );

    agent->setProperty( property_name , property_value );
    return this->getProperty( NEXTS ).toArray();

}
