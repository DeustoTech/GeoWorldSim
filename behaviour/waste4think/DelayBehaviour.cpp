#include "DelayBehaviour.h"

QString DelayBehaviour::NEXTS = "nexts";

DelayBehaviour::DelayBehaviour() : GWSBehaviour(){

}


QStringList DelayBehaviour::behave(){
    return this->getNexts( NEXTS );
}
