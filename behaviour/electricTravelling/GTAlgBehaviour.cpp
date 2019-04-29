#include "GTAlgBehaviour.h"

QString GTAlgBehaviour::VEHICLE_TYPE = "vehicle_type";
QString GTAlgBehaviour::STORE_NOISE_AS = "store_noise_as";
QString GTAlgBehaviour::NEXTS = "nexts";


GTAlgBehaviour::GTAlgBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > GTAlgBehaviour::behave(){


    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}

