#include "SetEntityPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString SetEntityPropertyBehaviour::ENTITY_ID = "agent_to_set_id";
QString SetEntityPropertyBehaviour::PROPERTIES = "properties";
QString SetEntityPropertyBehaviour::NEXTS = "nexts";

SetEntityPropertyBehaviour::SetEntityPropertyBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > SetEntityPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( ENTITY_ID ).toString();
    QSharedPointer<GWSEntity> agent = GWSEntityEnvironment::globalInstance()->getByUID( agent_id );
    
    if( !agent ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

    }

    QJsonObject properties = this->getProperty( PROPERTIES ).toObject();
    
    foreach ( QString property_name, properties.keys() ){

            QJsonValue property_value = properties.value( property_name );
            agent->setProperty( property_name , property_value );
        
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
