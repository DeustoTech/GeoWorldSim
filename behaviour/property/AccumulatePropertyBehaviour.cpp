#include "AccumulatePropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../object/ObjectFactory.h"

QString geoworldsim::behaviour::AccumulatePropertyBehaviour::ENTITY_ID = "entity_id";
QString geoworldsim::behaviour::AccumulatePropertyBehaviour::ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::AccumulatePropertyBehaviour::PROPERTIES = "properties";
QString geoworldsim::behaviour::AccumulatePropertyBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::AccumulatePropertyBehaviour::AccumulatePropertyBehaviour(): Behaviour(){

}

QPair< double , QJsonArray > geoworldsim::behaviour::AccumulatePropertyBehaviour::behave(){

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
    QJsonObject accumulations;
    foreach( QString property_name , properties.keys() ) {
        accumulations[ property_name ] = ObjectFactory::globalInstance()->simpleOrParentPropertyValue( properties.value( property_name ) , this->getSharedPointer() , behaving_entity );
    }

    foreach ( QSharedPointer< Entity > entity , entities ) {
        foreach( QString property_name , properties.keys() ) {

            QJsonValue value = entity->getProperty( property_name );

            if( accumulations[ property_name ].isNull() || accumulations[ property_name ].isUndefined() ){
                accumulations[ property_name ] = value;
                continue;
            }

            if( value.isString() ){
                accumulations[ property_name ] = accumulations[ property_name ].toString() + value.toString();
                continue;
            }

            accumulations[ property_name ] = accumulations[ property_name ].toDouble() + value.toDouble();

        }
    }

    behaving_entity->setProperties( accumulations );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
