#include "SetPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::SetPropertyBehaviour::ENTITY_ID = "entity_id";
QString geoworldsim::behaviour::SetPropertyBehaviour::ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::SetPropertyBehaviour::PROPERTIES = "properties";
QString geoworldsim::behaviour::SetPropertyBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::SetPropertyBehaviour::SetPropertyBehaviour() : Behaviour(){
}


QPair<double, QJsonArray> geoworldsim::behaviour::SetPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();

    QList< QSharedPointer< Entity > > entities;

    QSharedPointer< Entity > behaving_entity = this->getEntity();

    if( behaving_entity->getUID() == entity_id ){
        entities.append( behaving_entity );
    }

    // If ID and Type supplied
    else {
        entities = geoworldsim::environment::EntityEnvironment::globalInstance()->getMatches< Entity >( entity_type , entity_id );
    }

    QJsonObject properties = this->getProperty( PROPERTIES ).toObject();
    
    foreach (QSharedPointer< Entity > e , entities ) {
        if( !e ){ continue; }

        foreach( QString property_name , properties.keys() ){

                QJsonValue property_value = properties.value( property_name );
                e->setProperty( property_name , property_value );

        }
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
