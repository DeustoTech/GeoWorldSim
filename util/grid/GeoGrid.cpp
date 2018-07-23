#include "GeoGrid.h"

QString GWSGeoGrid::LEFT_BOUNDS_PROP = "bounds_left";
QString GWSGeoGrid::RIGHT_BOUNDS_PROP = "bounds_right";
QString GWSGeoGrid::TOP_BOUNDS_PROP = "bounds_top";
QString GWSGeoGrid::BOTTOM_BOUNDS_PROP = "bounds_bottom";

GWSGeoGrid::GWSGeoGrid() : GWSGrid(){

}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSPolygon* GWSGeoGrid::getGridBounds() const{

}

double GWSGeoGrid::getCellValue(const GWSPoint *point) const{

}

GWSPolygon* GWSGeoGrid::getCellEnvelope(unsigned int grid_x, unsigned int grid_y) const{

}
