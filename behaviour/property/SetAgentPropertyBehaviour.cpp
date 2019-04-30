#include "SetAgentPropertyBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

QString SetAgentPropertyBehaviour::AGENT_ID = "agent_to_set_id";
QString SetAgentPropertyBehaviour::PROPERTIES = "properties";
QString SetAgentPropertyBehaviour::NEXTS = "nexts";

SetAgentPropertyBehaviour::SetAgentPropertyBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > SetAgentPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( AGENT_ID ).toString();
    QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getByUID( agent_id );
    
    if( !agent ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

    }

    QJsonObject properties = this->getProperty( PROPERTIES ).toObject();
    
    foreach ( QString property_name, properties.keys() ){
        
            QJsonValue property_value = this->getProperty( property_name );
            agent->setProperty( property_name , property_value );
        
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
