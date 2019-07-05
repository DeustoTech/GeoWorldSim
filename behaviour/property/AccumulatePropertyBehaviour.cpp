#include "AccumulatePropertyBehaviour.h"

QString AccumulatePropertyBehaviour::ENTITY_ID = "entity_id";
QString AccumulatePropertyBehaviour::PROPERTIES = "properties";
QString AccumulatePropertyBehaviour::NEXTS = "nexts";

AccumulatePropertyBehaviour::AccumulatePropertyBehaviour(): GWSBehaviour(){

}

QPair< double , QJsonArray > AccumulatePropertyBehaviour::behave(){

   QString entity_id = this->getProperty( ENTITY_ID ).toString();
   QSharedPointer<GWSEntity> entity = GWSEntityEnvironment::globalInstance()->getByUID( entity_id );

   if( !entity ){
       return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
   }

   QJsonObject properties = this->getProperty( PROPERTIES ).toObject();

   foreach ( QString property_name, properties.keys() ){
           double existing_value = entity->getProperty( property_name ).toDouble();
           QJsonValue property_value = properties.value( property_name );
           entity->setProperty( property_name , GWSObjectFactory::incrementValue( existing_value , property_value ) );
   }

   return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
