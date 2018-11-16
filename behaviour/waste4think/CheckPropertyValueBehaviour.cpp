#include "CheckPropertyValueBehaviour.h"



QString CheckPropertyValueBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckPropertyValueBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE_NAME = "property_to_compare";
QString CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE_THRESHOLD_VALUE = "threshold_value";

CheckPropertyValueBehaviour::CheckPropertyValueBehaviour() : GWSBehaviour{}{

}


QStringList CheckPropertyValueBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double threshold_value = this->getProperty( PROPERTY_TO_COMPARE_THRESHOLD_VALUE).toDouble();

    // If it is a QJsonObject, we will need to sum up the quantities of each of the QJsonObjects within:
    QVariant property_to_compare = this->getProperty( PROPERTY_TO_COMPARE_NAME );
    QVariant agent_property_to_compare_value = agent->getProperty( property_to_compare.toString() );

    double total_property_amount = 0;

    if( agent_property_to_compare_value.typeName() == QString("QJsonObject") ){

        QJsonObject agent_property_to_compare_object = agent_property_to_compare_value.toJsonObject();

        foreach( QString key , agent_property_to_compare_object.keys() ){
            total_property_amount = total_property_amount + agent_property_to_compare_object[key].toDouble();
        }

    }


    QStringList nexts;

    if( total_property_amount >= threshold_value ){
         nexts = this->getNexts( NEXTS_IF_TRUE );
    }

    else {
        nexts = this->getNexts( NEXTS_IF_FALSE );
         }

    return nexts;
}
