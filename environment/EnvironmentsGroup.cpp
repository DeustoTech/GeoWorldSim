#include "EnvironmentsGroup.h"

geoworldsim::environment::EnvironmentsGroup* geoworldsim::environment::EnvironmentsGroup::globalInstance(){
    static geoworldsim::environment::EnvironmentsGroup instance;
    return &instance;
}

geoworldsim::environment::EnvironmentsGroup::EnvironmentsGroup() : Object(){
}

geoworldsim::environment::EnvironmentsGroup::~EnvironmentsGroup(){
}


/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::environment::EnvironmentsGroup::registerEntity( QSharedPointer< geoworldsim::Entity > entity){
    foreach (Environment* e, this->sub_environments ) {
        e->registerEntity( entity );
    }
}

void geoworldsim::environment::EnvironmentsGroup::unregisterEntity( QSharedPointer< geoworldsim::Entity > entity){

    foreach( Environment* e , this->sub_environments ){
        e->unregisterEntity( entity );
    }
}

void geoworldsim::environment::EnvironmentsGroup::addEnvironment(Environment *subenvironment){
    this->sub_environments.append( subenvironment );
}
