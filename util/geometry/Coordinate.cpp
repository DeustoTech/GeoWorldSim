#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

/**********************************************************************
 GETTERS
**********************************************************************/

double GWSCoordinate::getX() const{
    return this->x;
}

double GWSCoordinate::getY() const{
    return this->y;
}

double GWSCoordinate::getZ() const{
    return this->z;
}

GWSLengthUnit GWSCoordinate::getDistance(GWSCoordinate other) const {
    return GWSLengthUnit(
                geos::geom::Coordinate( this->x , this->y , this->z )
            .distance( geos::geom::Coordinate( other.x , other.y , other.z ) ) * 110574 );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSCoordinate::operator == (const GWSCoordinate& other) const{
    return this->x == other.x && this->y == other.y && this->z == other.z;
}

bool GWSCoordinate::operator != (const GWSCoordinate& other) const{
    return !(*this == other);
}

GWSCoordinate& GWSCoordinate::operator = (const GWSCoordinate& other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
}
