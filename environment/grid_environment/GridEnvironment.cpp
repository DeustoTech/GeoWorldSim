#include "GridEnvironment.h"

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "Grid environment created";
    this->environment_grids = new GWSObjectStorage( this );
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSGridEnvironment::~GWSGridEnvironment(){
    this->environment_grids->deleteLater();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerAgent( GWSAgent *agent ){
    GWSEnvironment::registerAgent( agent );
    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );

    if( grid ){
        this->environment_grids->add( agent );
    }
}

void GWSGridEnvironment::unregisterAgent( GWSAgent *agent ){
    GWSEnvironment::unregisterAgent( agent );
    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );
    if( grid ){
        this->environment_grids->remove( agent );
    }
}
