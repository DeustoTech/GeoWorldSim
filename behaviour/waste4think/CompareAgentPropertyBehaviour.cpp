#include "CompareAgentPropertyBehaviour.h"

#include "../../app/App.h"

QString CompareAgentPropertyBehaviour::AGENT_PROPERTY_TO_COMPARE = "agent_property_to_compare";
QString CompareAgentPropertyBehaviour::COMPARISON_OPERATOR = "comparison_operator";
QString CompareAgentPropertyBehaviour::COMPARISON_VALUE = "comparison_value";

QString CompareAgentPropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CompareAgentPropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CompareAgentPropertyBehaviour::CompareAgentPropertyBehaviour() : GWSBehaviour(){

}


QStringList CompareAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString property_to_compare = this->getProperty( AGENT_PROPERTY_TO_COMPARE ).toString();
    QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();
    QVariant comparison_value = this->getProperty( COMPARISON_VALUE );

    // If the comparison_value is written as '<PROP>' means that we need to fetch that value from an agent property
    bool is_property = comparison_value.toString().startsWith( "<" ) && comparison_value.toString().endsWith( ">" );
    if( is_property ){

        QString property_name = comparison_value.toString().remove(0,1);
        property_name = property_name.remove( property_name.length()-1 , 1 );
        comparison_value = agent->getProperty( property_name );
    }

    // Set boolean depending on comparison operator:
    bool comparison_fulfilled;

    if ( comparison_operator == "==" ){

        if ( agent->getProperty( property_to_compare ) == comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "!=" ){
        if ( agent->getProperty( property_to_compare ) != comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">=" ){
        if ( agent->getProperty( property_to_compare ) >= comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<=" ){
        if ( agent->getProperty( property_to_compare ) <= comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == ">" ){
        if ( agent->getProperty( property_to_compare ) > comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }

    if ( comparison_operator == "<" ){
        if ( agent->getProperty( property_to_compare ) < comparison_value ){
            comparison_fulfilled = true;
        } else {
             comparison_fulfilled = false;
        }
    }


    // Evaluate the according to boolean:
    if ( comparison_fulfilled ) {
        QStringList next = this->getProperty( NEXTS_IF_TRUE ).toStringList();
        return next;
    } else {
        QStringList next = this->getProperty( NEXTS_IF_FALSE ).toStringList();
        return next;
    }


}
