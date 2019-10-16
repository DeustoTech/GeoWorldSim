#include "Geometry.h"

#include <QJsonDocument>
#include <QJsonArray>
#include "GeometryToGeoJSON.h"
#include "GeometryGetters.h"
#include "GeometryComparators.h"

geoworldsim::geometry::Geometry::Geometry( const QJsonObject &geojson ) {
    this->geojson = QJsonObject( geojson );
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
    this->inner_geometry = GeometryToGeoJSON::fromGeoJSON( this->geojson );
}

geoworldsim::geometry::Geometry::Geometry( const geoworldsim::geometry::Coordinate &coor ){
    QJsonObject geojson;
    geojson.insert( "type" , "Point" );
    geojson.insert( "coordinates" , QJsonArray({ coor.getX() , coor.getY() , coor.getZ() }) );
    this->geojson = geojson;
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
    this->inner_geometry = GeometryToGeoJSON::fromGeoJSON( this->geojson );
}

geoworldsim::geometry::Geometry::Geometry( const geoworldsim::geometry::Geometry &other ) : geoworldsim::geometry::Geometry( other.geojson ){}

geoworldsim::geometry::Geometry::~Geometry(){
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::geometry::Geometry::getUID() const {
    return QJsonDocument( this->geojson ).toBinaryData();
}

QJsonObject geoworldsim::geometry::Geometry::getGeoJSON() const{
    return this->geojson;
}

bool geoworldsim::geometry::Geometry::isValid() const{
    return this->inner_geometry != Q_NULLPTR && !this->geojson.isEmpty();
}

geoworldsim::geometry::Coordinate geoworldsim::geometry::Geometry::getCentroid() const{
    if( this->isValid() ){
        geos::geom::Coordinate centroid;
        this->inner_geometry->getCentroid( centroid );
        return Coordinate( centroid.x , centroid.y , centroid.z );
    }
    return Coordinate();
}

geoworldsim::unit::LengthUnit geoworldsim::geometry::Geometry::getDistance(const geoworldsim::geometry::Geometry &other) const{
    return GeometryComparators::getDistance( *this , other );
}

geoworldsim::unit::LengthUnit geoworldsim::geometry::Geometry::getDistance(const geoworldsim::geometry::Coordinate &other) const{
    return GeometryComparators::getDistance( *this , other );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool geoworldsim::geometry::Geometry::operator == (const geoworldsim::geometry::Geometry& other) const {
    return this->geojson == other.geojson ;
}

bool geoworldsim::geometry::Geometry::operator != (const geoworldsim::geometry::Geometry& other) const{
    return !(*this == other);
}

bool geoworldsim::geometry::Geometry::operator < (const geoworldsim::geometry::Geometry& other) const{
    return this->getUID() < other.getUID();
}

geoworldsim::geometry::Geometry& geoworldsim::geometry::Geometry::operator = (const geoworldsim::geometry::Geometry& other){
    this->geojson = other.geojson;
    this->inner_geometry = GeometryToGeoJSON::fromGeoJSON( this->geojson );
    return *this;
}
