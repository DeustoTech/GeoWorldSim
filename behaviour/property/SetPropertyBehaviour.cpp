#include "SetPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::SetPropertyBehaviour::INPUT_ENTITY_ID = "entity_id";
QString geoworldsim::behaviour::SetPropertyBehaviour::INPUT_ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::SetPropertyBehaviour::INPUT_PROPERTIES = "properties";
QString geoworldsim::behaviour::SetPropertyBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::SetPropertyBehaviour::SetPropertyBehaviour() : Behaviour(){
}


QPair<double, QJsonArray> geoworldsim::behaviour::SetPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( INPUT_ENTITY_ID ).toString();
    QString entity_type = this->getProperty( INPUT_ENTITY_TYPE ).toString();

    QList<QSharedPointer< Entity > > entities;

    QSharedPointer< Entity > behaving_entity = this->getEntity();

    if( behaving_entity->getUID() == entity_id ){
        entities.append( behaving_entity );
    }

    // If ID and Type supplied
    else if( !entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByClassAndUID( entity_type , entity_id ) );
    }

    // If only Type supplied, affect all
    else if( entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByClass( entity_type ) );
    }

    // If only ID supplied
    else if ( !entity_id.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByUID( entity_id ) );
    }

    QJsonObject properties = this->getProperty( INPUT_PROPERTIES ).toObject();
    
    foreach (QSharedPointer< Entity > e , entities ) {
        if( !e ){ continue; }

        foreach( QString property_name , properties.keys() ){

                QJsonValue property_value = properties.value( property_name );
                e->setProperty( property_name , property_value );

        }
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
