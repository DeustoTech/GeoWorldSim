#include "SetEntityPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString SetEntityPropertyBehaviour::ENTITY_ID = "entity_id";
QString SetEntityPropertyBehaviour::PROPERTIES = "properties";
QString SetEntityPropertyBehaviour::NEXTS = "nexts";

SetEntityPropertyBehaviour::SetEntityPropertyBehaviour() : GWSBehaviour(){

}


QPair<double, QJsonArray> SetEntityPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QSharedPointer<GWSEntity> entity = GWSEntityEnvironment::globalInstance()->getByUID( entity_id );
    
    if( !entity ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    QJsonObject properties = this->getProperty( PROPERTIES ).toObject();
    
    foreach ( QString property_name, properties.keys() ){

            QJsonValue property_value = properties.value( property_name );
            entity->setProperty( property_name , property_value );
        
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
