#include "InterpolationGrid.h"

#include "utils/conversors/grid_coordinates/GridCoordinatesConversor.h"

InterpolationGrid::InterpolationGrid(double left, double right, double top, double bottom, unsigned int x_size, unsigned int y_size, double min_value, double max_value, GWSObject *parent) : GWSGrid( left , right , top , bottom , x_size , y_size , min_value , max_value , parent){
}

InterpolationGrid::InterpolationGrid(const InterpolationGrid &other) : GWSGrid( other ){
}

InterpolationGrid::InterpolationGrid(const GWSGrid &other) : GWSGrid( other ){
}

InterpolationGrid::InterpolationGrid(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

double InterpolationGrid::getValue(GWSCoordinate coor) const{

    // Get value
    double value = GWSGrid::getValue( coor );
    if( value <= this->getMinValue() - 1 ){
        return value;
    }

    // TODO

     return value;
}

