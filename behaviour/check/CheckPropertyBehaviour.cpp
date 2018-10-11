#include "CheckPropertyBehaviour.h"

QString CheckPropertyBehaviour::CHECK_PROPERTY_NAME_PROP = "property_name";
QString CheckPropertyBehaviour::CHECK_PROPERTY_VALUE_PROP = "check_value";

CheckPropertyBehaviour::CheckPropertyBehaviour() : GWSBehaviour(){
}

bool CheckPropertyBehaviour::canContinueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QVariant compare_value = this->getProperty( CHECK_PROPERTY_VALUE_PROP );
    QVariant agent_property = agent->getProperty( this->getProperty( CHECK_PROPERTY_NAME_PROP ).toString() );

    if( agent_property == compare_value ){
        return true;
    }

    return false;
}

bool CheckPropertyBehaviour::behave(){
    // EMPTY, DO NOTHING
    return true;
}
