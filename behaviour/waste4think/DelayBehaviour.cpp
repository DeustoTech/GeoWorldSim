#include "DelayBehaviour.h"

QString DelayBehaviour::NEXTS = "nexts";

DelayBehaviour::DelayBehaviour() : GWSBehaviour(){

}


QJsonArray DelayBehaviour::behave(){
    return this->getProperty( NEXTS ).toArray();
}
