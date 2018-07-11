#include "Geometry.h"

#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Polygon.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>
#include <QDebug>

#include "util/geometry/Point.h"
#include "util/geometry/LineString.h"
#include "util/geometry/Polygon.h"

#include "util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSGeometry::GWSGeometry(geos::geom::Geometry *geom, QObject *parent) :  QObject(parent) , inner_geometry( geom ){
}

GWSGeometry::~GWSGeometry(){
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = 0;
    }
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSGeometry::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color, QImage icon) const{
    Q_UNUSED(fill_color);
    Q_UNUSED(icon);

    // Image to be returned
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    if( !this->inner_geometry ){ return image; }

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );

    // Just draw representative coordinate
    QPoint draw = ImageCoordinatesConversor::reprojectPoint(
                this->getRepresentativeCoordinate().getY(),
                this->getRepresentativeCoordinate().getX(),
                image_bounds.getMaxY(),
                image_bounds.getMinY(),
                image_bounds.getMaxX(),
                image_bounds.getMinX(),
                image_width,
                image_height );

    QPen pen = QPen( border_color );
    pen.setWidth( image_width / 50 );
    painter.setPen( pen );
    painter.drawPoint( draw );
*/
    return image;
}

QJsonObject GWSGeometry::toGeoJSON( QJsonObject properties ) const{

    // Geometry
    QJsonObject json_geometry;
    json_geometry.insert( "properties" , properties );

    if( !this->inner_geometry ){ return json_geometry; }

    try {

        // POINT
        if( dynamic_cast<const GWSPoint*>(this) ){
            return dynamic_cast<const GWSPoint*>( this )->toGeoJSON( properties );
        }

        // MULTIGEOM
        if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTIPOINT ||
                   this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTILINESTRING ||
                   this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON ) {

            if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTIPOINT ){ json_geometry.insert("type", QString("MultiPoint") ); }
            if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTILINESTRING ){ json_geometry.insert("type", QString("MultiLineString") ); }
            if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON ){ json_geometry.insert("type", QString("MultiPolygon") ); }

            QJsonArray geomCoordinates;

            for(unsigned int i = 0; i < this->inner_geometry->getNumGeometries(); i++){
                GWSGeometry* g = new GWSGeometry( this->inner_geometry->getGeometryN(i)->clone() );
                geomCoordinates.append( g->toGeoJSON().value("coordinates") );
                delete g;
            }
            json_geometry.insert("coordinates", geomCoordinates);

            // GEOMETRYCOLLECTION
        } else if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION ) {

            json_geometry.insert("type" , QString("GeometryCollection") );
            QJsonArray geometries;

            for(unsigned int i = 0; i < this->inner_geometry->getNumGeometries(); i++){
                GWSGeometry* g = new GWSGeometry( this->inner_geometry->getGeometryN( i )->clone() );
                geometries.append( g->toGeoJSON() );
                delete g;
            }
            json_geometry.insert( "geometries" , geometries);

        } else {
            qWarning() << QString("Geometry type %1 unknown").arg( QString::fromStdString( this->inner_geometry->getGeometryType() ) );
        }

    } catch ( ... ){
        qWarning() << QString("Geometry type %1 parsing crashed").arg( QString::fromStdString( this->inner_geometry->getGeometryType() ) );
    }

    return json_geometry;
}

QString GWSGeometry::toString() const{
    return QString::fromStdString( this->inner_geometry->toString() );
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSCoordinate GWSGeometry::getRepresentativeCoordinate() const{
    return GWSCoordinate( this->inner_geometry->getCoordinate()->x , this->inner_geometry->getCoordinate()->y , this->inner_geometry->getCoordinate()->z );
}

GWSEnvelope GWSGeometry::getEnvelope() const{
    return GWSEnvelope( this->inner_geometry->getEnvelopeInternal()->getMinX() , this->inner_geometry->getEnvelopeInternal()->getMaxX() , this->inner_geometry->getEnvelopeInternal()->getMinY() , this->inner_geometry->getEnvelopeInternal()->getMaxY() );
}

QList<GWSCoordinate> GWSGeometry::getCoordinates() const{
    QList<GWSCoordinate> coors;
    for( unsigned int i = 0; i < this->inner_geometry->getCoordinates()->size() ; i++ ){
        coors.append( GWSCoordinate( this->inner_geometry->getCoordinates()->getAt( i ).x , this->inner_geometry->getCoordinates()->getAt( i ).y , this->inner_geometry->getCoordinates()->getAt( i ).z ) );
    }
    return coors;
}

GWSCoordinate GWSGeometry::getFirstCoordinate() const{
    geos::geom::Coordinate c = this->inner_geometry->getCoordinates()->getAt( 0 );
    return GWSCoordinate( c.x , c.y , c.z );
}

GWSCoordinate GWSGeometry::getLastCoordinate() const{
    geos::geom::Coordinate c = this->inner_geometry->getCoordinates()->getAt( this->inner_geometry->getCoordinates()->size()-1 );
    return GWSCoordinate( c.x , c.y , c.z );
}

bool GWSGeometry::isValid() const{
    return this->inner_geometry ? this->inner_geometry->isValid() : false;
}

bool GWSGeometry::intersects( const GWSGeometry *other ) const{
    return this->inner_geometry->intersects( other->inner_geometry );
}

bool GWSGeometry::equals( const GWSGeometry *other) const{
    return this->inner_geometry->equals( other->inner_geometry );
}

GWSAreaUnit GWSGeometry::getArea() const{
    return GWSAreaUnit( this->inner_geometry ? this->inner_geometry->getArea() : -1 );
}

GWSCoordinate GWSGeometry::getCentroid() const{
    if( this && this->inner_geometry ){
        geos::geom::Coordinate centroid;
        this->inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    return GWSCoordinate( 0 , 0 , 0 );
}

double GWSGeometry::getDistance( const GWSGeometry *other) const{
    return this->inner_geometry ? this->inner_geometry->distance( other->inner_geometry ) * 110574 : -1;
}

/**********************************************************************
 SETTERS
**********************************************************************/


/**********************************************************************
 METHODS
**********************************************************************/

GWSGeometry* GWSGeometry::createCopy() const{
    if( this && this->inner_geometry ){
        return this->castToGeometryType( this->inner_geometry->clone() );
    }
    return 0;
}

GWSPoint* GWSGeometry::createCentroid() const{
    return new GWSPoint( this->inner_geometry->getCentroid() );
}

GWSGeometry* GWSGeometry::createBuffer( double buffer ) const{
    return this->castToGeometryType( this->inner_geometry->buffer( buffer ) );
}

GWSGeometry* GWSGeometry::createUnion(GWSGeometry *other) const{
    return this->castToGeometryType( this->inner_geometry->Union( other->inner_geometry ) );
}

GWSGeometry* GWSGeometry::createIntersection(const GWSGeometry *other) const{
    return this->castToGeometryType( this->inner_geometry->intersection( other->inner_geometry ) );
}


/**********************************************************************
 PRIVATE
**********************************************************************/

GWSGeometry* GWSGeometry::castToGeometryType( geos::geom::Geometry *geom ) const{
    if( geom->getGeometryTypeId() == geos::geom::GEOS_POINT ){
        return new GWSPoint( dynamic_cast<geos::geom::Point*>(geom) );
    }
    if( geom->getGeometryTypeId() == geos::geom::GEOS_LINESTRING ){
        return new GWSLineString( dynamic_cast<geos::geom::LineString*>(geom) );
    }
    if( geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON ){
        return new GWSPolygon( dynamic_cast<geos::geom::Polygon*>(geom) );
    }
    return new GWSGeometry( geom );
}
