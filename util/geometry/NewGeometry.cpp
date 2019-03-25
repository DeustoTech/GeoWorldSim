#include "NewGeometry.h"

#include <QJsonDocument>
#include "GeometryToGeoJSON.h"

GWSNewGeometry::GWSNewGeometry( QJsonObject geojson ) {
    this->geojson = geojson;
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
    this->inner_geometry = GWSGeometryToGeoJSON::GeometryFromGeoJSON( this->geojson );
}

GWSNewGeometry::~GWSNewGeometry(){
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNewGeometry::getUID() const{
    return QJsonDocument( this->geojson ).toBinaryData();
}

bool GWSNewGeometry::isValid() const{
    return !this->geojson.isEmpty();
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSNewGeometry::operator == (const GWSNewGeometry& other) const {
    return this->geojson == other.geojson ;
}

bool GWSNewGeometry::operator != (const GWSNewGeometry& other) const{
    return !(*this == other);
}

bool GWSNewGeometry::operator < (const GWSNewGeometry& other) const{
    return this->getUID() < other.getUID();
}

GWSNewGeometry& GWSNewGeometry::operator = (const GWSNewGeometry& other){
    this->geojson = other.geojson;
    return *this;
}
