#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSCoordinate::isValid() const{
    return !(this->x != this->x || this->y != this->y || this->z != this->z);
}

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
    return GWSLengthUnit( this->getLatLngDistance( other) * 110574 );
}

double GWSCoordinate::getLatLngDistance(GWSCoordinate other) const{
    return geos::geom::Coordinate( this->x , this->y , this->z )
            .distance( geos::geom::Coordinate( other.x , other.y , other.z ) );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSCoordinate::operator == (const GWSCoordinate& other) const {
    return this->x == other.x && this->y == other.y && ( this->z != this->z || other.z != other.z ? true : this->z == other.z );
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
