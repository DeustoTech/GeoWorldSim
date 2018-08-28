#include "AgentGrid.h"

#include <QJsonArray>
#include <QDebug>

#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/grid/GridCoordinatesConversor.h"

GWSAgentGrid::GWSAgentGrid(GWSAgent *agent) : GWSGrid( agent ){

}

/**********************************************************************
 IMPORTERS
**********************************************************************/


/**********************************************************************
 EXPORTERS
**********************************************************************/



/**********************************************************************
 GETTERS
**********************************************************************/

QList<GWSAgent*> GWSAgentGrid::getGridCellValue(unsigned int grid_x, unsigned int grid_y){
    QList<GWSAgent*> agents;
    if( grid_x < 0 || grid_x > this->getGridXSize() || grid_y < 0 || grid_y > this->getGridYSize() ){
        return agents;
    }

    const GWSGeometry* grid_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() );
    double minX = grid_geom->getGeometryMinX();
    double maxX = grid_geom->getGeometryMaxX();
    double minY = grid_geom->getGeometryMinY();
    double maxY = grid_geom->getGeometryMaxY();

    double cell_minX = GWSGridCoordinatesConversor::x2lon( grid_x , minX , maxX , this->getGridXSize() );
    double cell_maxX = GWSGridCoordinatesConversor::x2lon( grid_x+1 , minX , maxX , this->getGridXSize() );
    double cell_minY = GWSGridCoordinatesConversor::x2lon( grid_y , minY , maxY , this->getGridYSize() );
    double cell_maxY = GWSGridCoordinatesConversor::x2lon( grid_y+1 , minY , maxY , this->getGridYSize() );

    foreach (GWSAgent* agent , this->agents_inside) {
        const GWSGeometry* agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent->getId() );
        if( agent_geom && agent_geom->intersects( cell_minX , cell_maxX , cell_minY , cell_maxY ) ){
            agents.append( agent );
        }
    }
    return agents;
}

/**********************************************************************
 SETTERS
**********************************************************************/

/*void GWSAgentGrid::addGridCellValue(unsigned int grid_x, unsigned int grid_y, GWSAgent *v){
    if( grid_x < 0 || grid_x >= this->getGridXSize() || grid_y < 0 || grid_y >= this->getGridYSize() ){
        return;
    }
    this->mutex.lock();
    this->values[grid_x][grid_y].append( v );
    this->mutex.unlock();
}

void GWSAgentGrid::removeGridCellValue(unsigned int grid_x, unsigned int grid_y, GWSAgent *v){
    if( grid_x < 0 || grid_x >= this->values.size() || grid_y < 0 || grid_y >= this->values[0].size() ){
        return;
    }
    this->mutex.lock();
    this->values[grid_x][grid_y].removeAll( v );
    this->mutex.unlock();
}*/

void GWSAgentGrid::enter(GWSAgent *agent){
    this->agents_inside.append( agent );
}

void GWSAgentGrid::exit(GWSAgent *agent){
    this->agents_inside.removeAll( agent );
}
