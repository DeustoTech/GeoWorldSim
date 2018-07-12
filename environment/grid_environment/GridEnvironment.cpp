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

        const QMetaObject* class_type = agent->metaObject();
        class_type = agent->metaObject();
        while( class_type ){

            // Add grid
            this->grids.insert( class_type->className() , grid );
            class_type = class_type->superClass();
        }
    }
}

void GWSGridEnvironment::unregisterAgent( GWSAgent *agent ){
    GWSGrid* grid = dynamic_cast<GWSGrid*>( agent );

    if( grid ){

        const QMetaObject* class_type = agent->metaObject();
        while( class_type ){

            // Remove grid
            this->grids.remove( class_type->className() );

            class_type = class_type->superClass();
        }
    }
}
