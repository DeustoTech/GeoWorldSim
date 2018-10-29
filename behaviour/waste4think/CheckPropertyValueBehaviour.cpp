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
    QVariant property_name = this->getProperty( PROPERTY_TO_COMPARE_NAME );

    bool is_property = property_name.toString().startsWith( "<" ) && property_name.toString().endsWith( ">" );

    if ( is_property ){
        QString property = property_name.toString().remove( 0 , 1 );
        property = property.remove( property.length()  - 1 , 1 );
        property_name = agent->getProperty( property );
    }

    //QString property_to_compare = property_name.toString();


    QStringList nexts;


    if( property_name >= threshold_value ){
         nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();
    }

    else {
        nexts = this->getProperty( NEXTS_IF_FALSE ).toStringList();
         }

    return nexts;
}
