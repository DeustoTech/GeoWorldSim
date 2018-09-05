#include "EnvironmentsGroup.h"

GWSEnvironmentsGroup* GWSEnvironmentsGroup::globalInstance(){
    static GWSEnvironmentsGroup instance;
    return &instance;
}

GWSEnvironmentsGroup::GWSEnvironmentsGroup() : GWSObject(){
}

GWSEnvironmentsGroup::~GWSEnvironmentsGroup(){
}


/**********************************************************************
 METHODS
**********************************************************************/

void GWSEnvironmentsGroup::registerAgent( QSharedPointer<GWSAgent> agent){
    foreach (GWSEnvironment* e, this->sub_environments ) {
        e->registerAgent( agent );
    }
}

void GWSEnvironmentsGroup::unregisterAgent( QSharedPointer<GWSAgent> agent){

    foreach( GWSEnvironment* e , this->sub_environments ){
        e->unregisterAgent( agent );
    }
}

void GWSEnvironmentsGroup::addEnvironment(GWSEnvironment *subenvironment){
    this->sub_environments.append( subenvironment );
}
