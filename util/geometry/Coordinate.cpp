#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

QString GWSCoordinate::X_PROP = "x";
QString GWSCoordinate::Y_PROP = "y";
QString GWSCoordinate::Z_PROP = "z";

GWSCoordinate::GWSCoordinate() : GWSObject( Q_NULLPTR ){
}

GWSCoordinate::GWSCoordinate( double x, double y, double z ) : GWSObject( Q_NULLPTR ){
    this->setProperty( GWSCoordinate::X_PROP , x );
    this->setProperty( GWSCoordinate::Y_PROP , y );
    this->setProperty( GWSCoordinate::Z_PROP , z );
}

GWSCoordinate::GWSCoordinate(const GWSCoordinate &other) : GWSObject( Q_NULLPTR ){
    this->setProperty( GWSCoordinate::X_PROP , other.getX() );
    this->setProperty( GWSCoordinate::Y_PROP , other.getY() );
    this->setProperty( GWSCoordinate::Z_PROP , other.getZ() );
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSCoordinate::isNull() const{
    return this->property( X_PROP ).isNull() || this->property( Y_PROP ).isNull() || this->property( Z_PROP ).isNull();
}

double GWSCoordinate::getX() const{
    return this->property( X_PROP ).toDouble();
}

double GWSCoordinate::getY() const{
    return this->property( Y_PROP ).toDouble();
}

double GWSCoordinate::getZ() const{
    return this->property( Z_PROP ).toDouble();
}

double GWSCoordinate::distance(GWSCoordinate other) const{
    return geos::geom::Coordinate( this->getX() , this->getY() , this->getZ() )
            .distance( geos::geom::Coordinate( other.getX() , other.getY() , other.getZ() ) );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSCoordinate::operator == (const GWSCoordinate& other) const{
    return this->getX() == other.getX() && this->getY() == other.getY() && this->getZ() == other.getZ();
}

bool GWSCoordinate::operator != (const GWSCoordinate& other) const{
    return !(*this == other);
}

GWSCoordinate& GWSCoordinate::operator = (const GWSCoordinate& other){
    this->setProperty( X_PROP , other.getX() );
    this->setProperty( Y_PROP , other.getY() );
    this->setProperty( Z_PROP , other.getZ() );
    return *this;
}
