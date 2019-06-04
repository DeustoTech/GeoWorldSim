#include "CheckPropertyValueBehaviour.h"



QString CheckPropertyValueBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckPropertyValueBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE = "properties";

CheckPropertyValueBehaviour::CheckPropertyValueBehaviour() : GWSBehaviour{}{

}

QPair< double , QJsonArray > CheckPropertyValueBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();

    // If it is a QJsonObject, we will need to sum up the quantities of each of the QJsonObjects within:
    QJsonObject property_to_compare = this->getProperty( PROPERTY_TO_COMPARE ).toObject();


    if( entity->fulfillsFilter( property_to_compare , false ) ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
}
