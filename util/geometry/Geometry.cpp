#include "Geometry.h"

#include <QJsonDocument>
#include <QJsonArray>
#include "GeometryToGeoJSON.h"
#include "GeometryGetters.h"
#include "GeometryComparators.h"

GWSGeometry::GWSGeometry( const QJsonObject &geojson ) {
    this->geojson = QJsonObject( geojson );
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
    this->inner_geometry = GWSGeometryToGeoJSON::GeometryFromGeoJSON( this->geojson );
}

GWSGeometry::GWSGeometry( const GWSCoordinate &coor ){
    QJsonObject geojson;
    geojson.insert( "type" , "Point" );
    geojson.insert( "coordinates" , QJsonArray({ coor.getX() , coor.getY() , coor.getZ() }) );
    this->geojson = geojson;
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
    this->inner_geometry = GWSGeometryToGeoJSON::GeometryFromGeoJSON( this->geojson );
}

GWSGeometry::GWSGeometry(const GWSGeometry &other ) : GWSGeometry( other.geojson ){}

GWSGeometry::~GWSGeometry(){
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSGeometry::getUID() const {
    return QJsonDocument( this->geojson ).toBinaryData();
}

QJsonObject GWSGeometry::getGeoJSON() const{
    return this->geojson;
}

bool GWSGeometry::isValid() const{
    return this->inner_geometry != Q_NULLPTR && !this->geojson.isEmpty();
}

GWSCoordinate GWSGeometry::getCentroid() const{
    if( this->isValid() ){
        geos::geom::Coordinate centroid;
        this->inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    qDebug() << 1;
    return GWSCoordinate();
}

GWSLengthUnit GWSGeometry::getDistance(const GWSGeometry &other) const{
    return GWSGeometryComparators::getDistance( *this , other );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSGeometry::operator == (const GWSGeometry& other) const {
    return this->geojson == other.geojson ;
}

bool GWSGeometry::operator != (const GWSGeometry& other) const{
    return !(*this == other);
}

bool GWSGeometry::operator < (const GWSGeometry& other) const{
    return this->getUID() < other.getUID();
}

GWSGeometry& GWSGeometry::operator = (const GWSGeometry& other){
    this->geojson = other.geojson;
    return *this;
}
