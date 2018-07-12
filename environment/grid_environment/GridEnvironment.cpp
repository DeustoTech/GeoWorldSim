#include "GridEnvironment.h"

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "GWSGridEnvironment created";
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

        QStringList classes = agent->getInheritanceTree();
        foreach(QString c , classes){

            // Add grid
            this->grids.insert( c , grid );
        }
    }
}

void GWSGridEnvironment::unregisterAgent( GWSAgent *agent ){
    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );

    if( grid ){

        QStringList classes = agent->getInheritanceTree();
        foreach(QString c , classes){

            // Remove grid
            this->grids.remove( c );

        }
    }
}
