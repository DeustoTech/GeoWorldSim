#include "CheckPropertyValueBehaviour.h"



QString CheckPropertyValueBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckPropertyValueBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE_NAME = "property_to_compare";
QString CheckPropertyValueBehaviour::REFERENCE_VALUE_TO_COMPARE = "threshold_value";

CheckPropertyValueBehaviour::CheckPropertyValueBehaviour() : GWSBehaviour{}{

}


QJsonArray CheckPropertyValueBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QJsonValue threshold_value = this->getProperty( REFERENCE_VALUE_TO_COMPARE);

    // If it is a QJsonObject, we will need to sum up the quantities of each of the QJsonObjects within:
    QJsonValue property_name = this->getProperty( PROPERTY_TO_COMPARE_NAME );
    QJsonValue agent_property_value = agent->getProperty( property_name.toString() );

    double total = 0;

    switch ( agent_property_value.type() ) {
    case QJsonValue::Double : {
        total = agent_property_value.toDouble();
        break;
    }
    case QJsonValue::Array : {

        QJsonArray valueArray = agent_property_value.toArray();
        for ( int i = 0 ; i < valueArray.size() ; i++){
            total += valueArray.at(i).toDouble();
        }
        break;
    }
    case QJsonValue::Object : {

        QJsonArray valueArray;
        QJsonObject existing_object = agent_property_value.toObject();
        foreach( QString key , existing_object.keys() ){
                QJsonValue value = existing_object.value( key );
                double valueDouble = value.toDouble();
                valueArray.append(valueDouble);
            }

        for ( int i = 0 ; i < valueArray.size() ; i++){
            total += valueArray.at(i).toDouble();
        }

        break;
    }
    case QJsonValue::Null : {
        break;
    }
    }



    //








    qDebug() << total;

    bool comparison_success = threshold_value == total;

    if( comparison_success ){
         return this->getProperty( NEXTS_IF_TRUE ).toArray();
    } else {
        return this->getProperty( NEXTS_IF_FALSE ).toArray();
    }
}
