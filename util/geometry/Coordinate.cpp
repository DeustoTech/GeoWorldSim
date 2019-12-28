#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::geometry::Coordinate::getUID() const{
    return this->toString();
}

bool geoworldsim::geometry::Coordinate::isValid() const{
    return !(this->x != this->x || this->y != this->y ) // Checks NAN
           && this->x != Q_INFINITY && this->y != Q_INFINITY && this->z != Q_INFINITY; // Checks INFINITY
}

double geoworldsim::geometry::Coordinate::getX() const{
    return this->x;
}

double geoworldsim::geometry::Coordinate::getY() const{
    return this->y;
}

double geoworldsim::geometry::Coordinate::getZ() const{
    return this->z;
}

geoworldsim::unit::LengthUnit geoworldsim::geometry::Coordinate::getDistance(const geoworldsim::geometry::Coordinate &other) const {
    return unit::LengthUnit(
                geos::geom::Coordinate( this->x , this->y , this->z )
                .distance( geos::geom::Coordinate( other.x , other.y , other.z ) ) * 110574 ); // MAGIC NUMBER
}

double geoworldsim::geometry::Coordinate::getLatLngDistance( const geoworldsim::geometry::Coordinate &other ) const{
    return geos::geom::Coordinate( this->x , this->y , this->z )
            .distance( geos::geom::Coordinate( other.x , other.y , other.z ) );
}

bool geoworldsim::geometry::Coordinate::isInBounds(double minX, double maxX, double minY, double maxY) const{
    return minX <= this->getX() && this->getX() <= maxX && minY <= this->getY() && this->getY() <= maxY;
}

QString geoworldsim::geometry::Coordinate::toString() const{
    return QString("%1,%2,%3").arg( QString::number( this->getX() , 'g' , 7 ) ).arg( QString::number( this->getY() , 'g' , 7 ) ).arg( QString::number( this->getZ() , 'g' , 7 ) );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool geoworldsim::geometry::Coordinate::operator == (const geoworldsim::geometry::Coordinate& other) const {
    return (this->x == other.x) && (this->y == other.y) && ( this->z != this->z || other.z != other.z ? true : this->z == other.z );
}

bool geoworldsim::geometry::Coordinate::operator != (const geoworldsim::geometry::Coordinate& other) const{
    return !(*this == other);
}

bool geoworldsim::geometry::Coordinate::operator < (const geoworldsim::geometry::Coordinate& other) const{
    return this->x < other.x;
}

geoworldsim::geometry::Coordinate& geoworldsim::geometry::Coordinate::operator = (const geoworldsim::geometry::Coordinate& other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
}

geoworldsim::geometry::Coordinate& geoworldsim::geometry::Coordinate::operator+ (const geoworldsim::geometry::Coordinate& other){
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    return *this;
}

geoworldsim::geometry::Coordinate& geoworldsim::geometry::Coordinate::operator- (const geoworldsim::geometry::Coordinate& other){
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    return *this;
}
