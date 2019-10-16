#include "SetPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString SetPropertyBehaviour::ENTITY_ID = "entity_id";
QString SetPropertyBehaviour::ENTITY_TYPE = "entity_type";
QString SetPropertyBehaviour::PROPERTIES = "properties";
QString SetPropertyBehaviour::NEXTS = "nexts";

SetPropertyBehaviour::SetPropertyBehaviour() : GWSBehaviour(){

}


QPair<double, QJsonArray> SetPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();

    QList<QSharedPointer<GWSEntity> > entities;

    QSharedPointer<GWSEntity> behaving_entity = this->getEntity();

    if( behaving_entity->getUID() == entity_id ){
        entities.append( behaving_entity );
    }

    // If ID and Type supplied
    else if( !entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( GWSEntityEnvironment::globalInstance()->getByClassAndUID( entity_type , entity_id ) );
    }

    // If only Type supplied, affect all
    else if( entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( GWSEntityEnvironment::globalInstance()->getByClass( entity_type ) );
    }

    // If only ID supplied
    else if ( !entity_id.isEmpty() ){
        entities.append( GWSEntityEnvironment::globalInstance()->getByUID( entity_id ) );
    }

    QJsonObject properties = this->getProperty( PROPERTIES ).toObject();
    
    foreach (QSharedPointer<GWSEntity> e , entities ) {
        if( !e ){ continue; }

        foreach( QString property_name , properties.keys() ){

                QJsonValue property_value = properties.value( property_name );
                e->setProperty( property_name , property_value );

        }
    }
    
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
