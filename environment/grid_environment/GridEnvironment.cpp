#include "GridEnvironment.h"

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "Grid environment created";
    this->environment_grids = new GWSObjectStorage();
    this->environment_grids->setParent( this->getSharedPointer() );
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSGridEnvironment::~GWSGridEnvironment(){
    this->environment_grids->deleteLater();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){
    QSharedPointer<GWSGrid> grid = agent.dynamicCast<GWSGrid>();

    if( grid ){
        GWSEnvironment::registerAgent( agent );
        this->environment_grids->add( agent );
    }
}

void GWSGridEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){
    QSharedPointer<GWSGrid> grid = agent.dynamicCast<GWSGrid>();

    if( grid ){
        GWSEnvironment::unregisterAgent( agent );
        this->environment_grids->remove( agent );
    }
}
