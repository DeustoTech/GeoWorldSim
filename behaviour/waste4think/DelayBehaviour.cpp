#include "DelayBehaviour.h"

QString DelayBehaviour::NEXTS = "nexts";

DelayBehaviour::DelayBehaviour() : GWSBehaviour(){

}


QStringList DelayBehaviour::behave(){
    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
