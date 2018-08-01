#include "GridEnvironment.h"

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "Grid environment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSGridEnvironment::~GWSGridEnvironment(){
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerAgent( GWSAgent *agent ){

    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );

    if( grid ){
        this->environment_grids->add( agent );
    }
}

void GWSGridEnvironment::unregisterAgent( GWSAgent *agent ){

    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );
    if( grid ){
        this->environment_grids->remove( agent );
    }
}
