#include "Coordinate.h"

#include "geos/geom/Coordinate.h"

using namespace geos::geom;

QString GWSCoordinate::LATITUDE_PROP = "latitude";
QString GWSCoordinate::LONGITUDE_PROP = "longitude";
QString GWSCoordinate::ELEVATION_PROP = "elevation";

GWSCoordinate::GWSCoordinate( QObject* parent ) : GWSObject( parent ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSCoordinate::isNull() const{
    return this->getProperty( LATITUDE_PROP ).isNull() && this->getProperty( LONGITUDE_PROP ).isNull() && this->getProperty( ELEVATION_PROP ).isNull();
}

double GWSCoordinate::getLatitude() const{
    return this->latitude; //this->getProperty( LATITUDE_PROP ).toDouble();
}

double GWSCoordinate::getLongitude() const{
    return this->longitude; //this->getProperty( LONGITUDE_PROP ).toDouble();
}

double GWSCoordinate::getElevation() const{
    return this->altitude; //this->getProperty( ELEVATION_PROP ).toDouble();
}

double GWSCoordinate::getX() const{
    return this->getLongitude();
}

double GWSCoordinate::getY() const{
    return this->getLatitude();
}

double GWSCoordinate::getZ() const{
    return this->getElevation();
}

double GWSCoordinate::distance(GWSCoordinate* other) const {
    return geos::geom::Coordinate( this->getX() , this->getY() , this->getZ() )
            .distance( geos::geom::Coordinate( other->getX() , other->getY() , other->getZ() ) );
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
    this->setProperty( LATITUDE_PROP , other.getLatitude() );
    this->setProperty( LONGITUDE_PROP , other.getLongitude() );
    this->setProperty( ELEVATION_PROP , other.getElevation() );
    return *this;
}
