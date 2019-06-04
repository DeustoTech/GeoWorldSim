#include "MathEntityPropertyBehaviour.h"

QString MathEntityPropertyBehaviour::OPERATOR = "math_operator";
QString MathEntityPropertyBehaviour::OPERAND_PROPERTY = "property_to_modify";
QString MathEntityPropertyBehaviour::OPERAND_VALUE = "operand_value";
QString MathEntityPropertyBehaviour::ENTITY_ID_TO_OPERATE_ON = "entity_to_operate_on_id";
QString MathEntityPropertyBehaviour::NEXTS = "nexts";



MathEntityPropertyBehaviour::MathEntityPropertyBehaviour() : GWSBehaviour (){

}


QPair< double , QJsonArray > MathEntityPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID_TO_OPERATE_ON ).toString();
    QSharedPointer<GWSEntity> entity = GWSEntityEnvironment::globalInstance()->getByUID( entity_id );

    QString operation = this->getProperty( OPERATOR ).toString();
    QJsonValue operand_value = this->getProperty( OPERAND_VALUE );
    QString property_to_modify = this->getProperty( OPERAND_PROPERTY ).toString();

    double initial_property_value = entity->getProperty( property_to_modify ).toDouble();
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

    entity->setProperty( property_to_modify , final_property_value );
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
