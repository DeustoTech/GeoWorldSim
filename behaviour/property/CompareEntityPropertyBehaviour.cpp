#include "CompareEntityPropertyBehaviour.h"

#include "../../app/App.h"

QString CompareEntityPropertyBehaviour::ENTITY_PROPERTY_TO_COMPARE = "entity_property_to_compare";
QString CompareEntityPropertyBehaviour::COMPARISON_OPERATOR = "comparison_operator";
QString CompareEntityPropertyBehaviour::COMPARISON_VALUE = "comparison_value";

QString CompareEntityPropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CompareEntityPropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CompareEntityPropertyBehaviour::CompareEntityPropertyBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > CompareEntityPropertyBehaviour::behave(){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QString property_to_compare = this->getProperty( ENTITY_PROPERTY_TO_COMPARE ).toString();
    QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();
    QJsonValue comparison_value = this->getProperty( COMPARISON_VALUE );

    // Set boolean depending on comparison operator:
    bool comparison_fulfilled = false;

    QJsonValue property = agent->getProperty( property_to_compare );
    if( property.isNull() ){
       return  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
    }

    if ( comparison_operator == "==" ){

        if ( property == comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "!=" ){
        if ( property != comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">=" ){
        if ( property.toDouble() >= comparison_value.toDouble() ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<=" ){
        if ( property.toDouble() <= comparison_value.toDouble()){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">" ){
        if ( property.toDouble() > comparison_value.toDouble() ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<" ){
        if ( property.toDouble() < comparison_value.toDouble() ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
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
