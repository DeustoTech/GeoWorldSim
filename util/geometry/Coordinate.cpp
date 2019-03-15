#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSCoordinate::isValid() const{
    return !(this->x != this->x || this->y != this->y ) // Checks NAN
           && this->x != Q_INFINITY && this->y != Q_INFINITY && this->z != Q_INFINITY; // Checks INFINITY
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

bool GWSCoordinate::isInBounds(double minX, double maxX, double minY, double maxY) const{
    return minX <= this->getX() && this->getX() <= maxX && minY <= this->getY() && this->getY() <= maxY;
}

QString GWSCoordinate::toString() const{
    return QString("%1,%2,%3").arg( QString::number( this->getX() , 'g' , 7 ) ).arg( QString::number( this->getY() , 'g' , 7 ) ).arg( QString::number( this->getZ() , 'g' , 7 ) );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSCoordinate::operator == (const GWSCoordinate& other) const {
    return (this->x == other.x) && (this->y == other.y) && ( this->z != this->z || other.z != other.z ? true : this->z == other.z );
}

bool GWSCoordinate::operator != (const GWSCoordinate& other) const{
    return !(*this == other);
}

bool GWSCoordinate::operator < (const GWSCoordinate& other) const{
    return this->x < other.x;
}

GWSCoordinate& GWSCoordinate::operator = (const GWSCoordinate& other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
}

GWSCoordinate& GWSCoordinate::operator+ (const GWSCoordinate& other){
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    return *this;
}

GWSCoordinate& GWSCoordinate::operator- (const GWSCoordinate& other){
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    return *this;
}
