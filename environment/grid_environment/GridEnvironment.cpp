#include "GridEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "Grid environment created";
    this->environment_grids = new GWSObjectStorage();
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
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
