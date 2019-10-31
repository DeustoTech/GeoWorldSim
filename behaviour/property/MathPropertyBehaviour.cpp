#include "MathPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString MathPropertyBehaviour::ENTITY_ID = "entity_id";
QString MathPropertyBehaviour::ENTITY_TYPE = "entity_type";
QString MathPropertyBehaviour::OPERAND_PROPERTY = "property_to_modify";
QString MathPropertyBehaviour::OPERATOR = "math_operator";
QString MathPropertyBehaviour::OPERAND_VALUE = "operand_value";
QString MathPropertyBehaviour::NEXTS = "nexts";


MathPropertyBehaviour::MathPropertyBehaviour() : GWSBehaviour (){

}


QPair< double , QJsonArray > MathPropertyBehaviour::behave(){

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

    foreach (QSharedPointer<GWSEntity> e , entities ) {
        if( !e ){ continue; }

        QString operation = this->getProperty( OPERATOR ).toString();
        QJsonValue operand_value = this->getProperty( OPERAND_VALUE );
        QString property_to_modify = this->getProperty( OPERAND_PROPERTY ).toString();

        double initial_property_value = e->getProperty( property_to_modify ).toDouble();
        double final_property_value;

        if ( operation == "+" ){
            final_property_value = initial_property_value + operand_value.toDouble();
        }
        if ( operation == "/" ){
            final_property_value = initial_property_value / operand_value.toDouble();
        }
        if ( operation == "-" ){
            final_property_value = initial_property_value - operand_value.toDouble();

        }
        if ( operation == "*" ){
            final_property_value = initial_property_value * operand_value.toDouble();
        }

        e->setProperty( property_to_modify , final_property_value );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
