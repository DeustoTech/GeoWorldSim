#include "CompareAgentPropertyBehaviour.h"

#include "../../app/App.h"

QString CompareAgentPropertyBehaviour::AGENT_PROPERTY_TO_COMPARE = "agent_property_to_compare";
QString CompareAgentPropertyBehaviour::COMPARISON_OPERATOR = "comparison_operator";
QString CompareAgentPropertyBehaviour::COMPARISON_VALUE = "comparison_value";
QString CompareAgentPropertyBehaviour::COMPARISON_PROPERTY = "comparison_property";
QString CompareAgentPropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CompareAgentPropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CompareAgentPropertyBehaviour::CompareAgentPropertyBehaviour() : GWSBehaviour(){

}


QStringList CompareAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString property_to_compare = this->getProperty( AGENT_PROPERTY_TO_COMPARE ).toString();
    QVariant comparison_value = this->getProperty( COMPARISON_VALUE );
    QString comparison_property = this->getProperty( COMPARISON_PROPERTY ).toString();
    QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();


    // Set boolean depending on comparison operator:
    bool comparison_fulfilled;

    if ( comparison_operator == "==" ){
        if ( agent->getProperty( property_to_compare ) == comparison_value || agent->getProperty( property_to_compare ) == comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "!=" ){
        if ( agent->getProperty( property_to_compare ) != comparison_value || agent->getProperty( property_to_compare ) != comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">=" ){
        if ( agent->getProperty( property_to_compare ) >= comparison_value || agent->getProperty( property_to_compare ) >= comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<=" ){
        if ( agent->getProperty( property_to_compare ) <= comparison_value || agent->getProperty( property_to_compare ) <= comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">" ){
        if ( agent->getProperty( property_to_compare ) > comparison_value || agent->getProperty( property_to_compare ) > comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<" ){
        if ( agent->getProperty( property_to_compare ) < comparison_value || agent->getProperty( property_to_compare ) < comparison_property ){
            comparison_fulfilled = true;
        }
        else {
             comparison_fulfilled = false;
        }
    }


    // Evaluate the according to boolean:
    if ( comparison_fulfilled ) {
        QStringList next = this->getProperty( NEXTS_IF_TRUE ).toStringList();
        return next;
    }

    else {
        QStringList next = this->getProperty( NEXTS_IF_FALSE ).toStringList();
        return next;
    }


}
