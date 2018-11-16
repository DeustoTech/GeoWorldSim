#include "CheckPropertyValueBehaviour.h"



QString CheckPropertyValueBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckPropertyValueBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE_NAME = "property_to_compare";
QString CheckPropertyValueBehaviour::REFERENCE_VALUE_TO_COMPARE = "reference_value";

CheckPropertyValueBehaviour::CheckPropertyValueBehaviour() : GWSBehaviour{}{

}


QJsonArray CheckPropertyValueBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QJsonValue reference_value = this->getProperty( REFERENCE_VALUE_TO_COMPARE);

    // If it is a QJsonObject, we will need to sum up the quantities of each of the QJsonObjects within:
    QJsonValue property_name = this->getProperty( PROPERTY_TO_COMPARE_NAME );
    QJsonValue agent_property_value = agent->getProperty( property_name.toString() );

    bool comparison_success = reference_value == agent_property_value;

    if( comparison_success ){
         return this->getProperty( NEXTS_IF_TRUE ).toArray();
    } else {
        return this->getProperty( NEXTS_IF_FALSE ).toArray();
    }
}
