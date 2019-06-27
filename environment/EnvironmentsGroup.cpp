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

void GWSEnvironmentsGroup::registerEntity( QSharedPointer<GWSEntity> entity){
    foreach (GWSEnvironment* e, this->sub_environments ) {
        e->registerEntity( entity );
    }
}

void GWSEnvironmentsGroup::unregisterEntity( QSharedPointer<GWSEntity> entity){

    foreach( GWSEnvironment* e , this->sub_environments ){
        e->unregisterEntity( entity );
    }
}

void GWSEnvironmentsGroup::addEnvironment(GWSEnvironment *subenvironment){
    this->sub_environments.append( subenvironment );
}
