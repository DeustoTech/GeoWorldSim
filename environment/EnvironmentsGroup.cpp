#include "EnvironmentsGroup.h"
#include <QtConcurrent/QtConcurrent>

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

void GWSEnvironmentsGroup::registerEntity( QSharedPointer<GWSEntity> agent){
    foreach (GWSEnvironment* e, this->sub_environments ) {
        e->registerEntity( agent );
    }
}

void GWSEnvironmentsGroup::unregisterAgent( QSharedPointer<GWSEntity> agent){

    foreach( GWSEnvironment* e , this->sub_environments ){
        e->unregisterEntity( agent );
    }
}

void GWSEnvironmentsGroup::addEnvironment(GWSEnvironment *subenvironment){
    this->sub_environments.append( subenvironment );
}
