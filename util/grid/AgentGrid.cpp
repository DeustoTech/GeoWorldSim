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

    const GWSGeometry* agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() );
    double minX = agent_geom->getGeometryMinX();
    double maxX = agent_geom->getGeometryMaxX();
    double minY = agent_geom->getGeometryMinY();
    double maxY = agent_geom->getGeometryMaxY();

    foreach (GWSAgent* agent , this->agents_inside) {
        if( agent_geom->intersects( minX , maxX , minY , maxY ) ){
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
