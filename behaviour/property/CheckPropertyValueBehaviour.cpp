#include "CheckPropertyValueBehaviour.h"

QString geoworldsim::behaviour::CheckPropertyValueBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString geoworldsim::behaviour::CheckPropertyValueBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString geoworldsim::behaviour::CheckPropertyValueBehaviour::PROPERTY_TO_COMPARE = "properties";

geoworldsim::behaviour::CheckPropertyValueBehaviour::CheckPropertyValueBehaviour() : Behaviour{}{
}

QPair< double , QJsonArray > geoworldsim::behaviour::CheckPropertyValueBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();

    // If it is a QJsonObject, we will need to sum up the quantities of each of the QJsonObjects within:
    QJsonObject property_to_compare = this->getProperty( PROPERTY_TO_COMPARE ).toObject();

    if( entity->fulfillsFilter( property_to_compare , false ) ){

        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE).toArray() );

    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
}
