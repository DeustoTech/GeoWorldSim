#include "ComparePropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString ComparePropertyBehaviour::ENTITY_ID = "entity_id";
QString ComparePropertyBehaviour::ENTITY_TYPE = "entity_type";
QString ComparePropertyBehaviour::COMPARE_PROPERTY = "compare_property";
QString ComparePropertyBehaviour::COMPARISON_OPERATOR = "comparison_operator";
QString ComparePropertyBehaviour::COMPARISON_VALUE = "comparison_value";

QString ComparePropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString ComparePropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


ComparePropertyBehaviour::ComparePropertyBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > ComparePropertyBehaviour::behave(){


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

    bool comparison_fulfilled = false;

    foreach( QSharedPointer<GWSEntity> e , entities ){
        if( !e ){ continue; }

        QString property_to_compare = this->getProperty( COMPARE_PROPERTY ).toString();
        QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();
        QJsonValue comparison_value = this->getProperty( COMPARISON_VALUE );

        QJsonValue property = e->getProperty( property_to_compare );
        if( property.isNull() ){
            return  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
        }

        // ==

        if ( comparison_operator == "==" ) {
            comparison_fulfilled = property == comparison_value ? true : false;
        }

        // !=

        if ( comparison_operator == "!=" ){
            comparison_fulfilled = property != comparison_value ? true : false;
        }

        // >=

        if ( comparison_operator == ">=" ){
            comparison_fulfilled = property.toDouble() >= comparison_value.toDouble() ? true : false;
        }

        // <=

        if ( comparison_operator == "<=" ){
            comparison_fulfilled = property.toDouble() <= comparison_value.toDouble() ? true : false;
        }

        // >

        if ( comparison_operator == ">" ){
            comparison_fulfilled = property.toDouble() > comparison_value.toDouble() ? true : false;
        }

        // <

        if ( comparison_operator == "<" ){
            comparison_fulfilled = property.toDouble() < comparison_value.toDouble() ? true : false;
        }

        if( !comparison_fulfilled ){
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
        }
    }

    // Evaluate the according to boolean:
    QPair< double , QJsonArray > next;
    if ( comparison_fulfilled ) {
        next =  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
    } else {
        next =  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
    }
    return next;

}
