#include "CompareAgentPropertyBehaviour.h"

#include "../../app/App.h"

QString CompareAgentPropertyBehaviour::AGENT_PROPERTY_TO_COMPARE = "agent_property_to_compare";
QString CompareAgentPropertyBehaviour::COMPARISON_OPERATOR = "comparison_operator";
QString CompareAgentPropertyBehaviour::COMPARISON_VALUE = "comparison_value";

QString CompareAgentPropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CompareAgentPropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CompareAgentPropertyBehaviour::CompareAgentPropertyBehaviour() : GWSBehaviour(){

}


QJsonArray CompareAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString property_to_compare = this->getProperty( AGENT_PROPERTY_TO_COMPARE ).toString();
    QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();
    QJsonValue comparison_value = this->getProperty( COMPARISON_VALUE );

    // Set boolean depending on comparison operator:
    bool comparison_fulfilled = false;

    QJsonValue property = agent->getProperty( property_to_compare );
    if( property.isNull() ){
        QJsonArray next = this->getProperty( NEXTS_IF_FALSE ).toArray();
        return next;
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
    QJsonArray next;
    if ( comparison_fulfilled ) {
        next = this->getProperty( NEXTS_IF_TRUE ).toArray();
    } else {
        next = this->getProperty( NEXTS_IF_FALSE ).toArray();
    }
    return next;

}
