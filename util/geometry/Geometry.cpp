#include "Geometry.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>
#include <QDebug>

#include "Point.h"

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/LineString.h"

//#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSGeometry::GWSGeometry() : GWSObject(){
}

GWSGeometry::GWSGeometry( const geos::geom::Geometry* inner_geometry ) : GWSObject(){
    this->inner_geometry = inner_geometry;
}

GWSGeometry::~GWSGeometry(){
    if( this->inner_geometry ){
        delete this->inner_geometry;
        this->inner_geometry = Q_NULLPTR;
    }
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGeometry::deserialize(QJsonObject json){

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/*QImage GWSGeometry::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color, QImage icon) const{
    Q_UNUSED(fill_color);
    Q_UNUSED(icon);

    // Image to be returned
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

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

    return image;
}*/

QJsonObject GWSGeometry::serialize() const{

    /*QJsonObject json_geometry;
    if( !this->inner_geometry ){ return json_geometry; }

    try {

        // POINT
        if( dynamic_cast<const GWSPoint*>(this) ){
            return dynamic_cast<const GWSPoint*>( this )->serialize();
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
                geomCoordinates.append( g->serialize().value("coordinates") );
                delete g;
            }
            json_geometry.insert("coordinates", geomCoordinates);

            // GEOMETRYCOLLECTION
        } else if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION ) {

            json_geometry.insert("type" , QString("GeometryCollection") );
            QJsonArray geometries;

            for(unsigned int i = 0; i < this->inner_geometry->getNumGeometries(); i++){
                GWSGeometry* g = new GWSGeometry( this->inner_geometry->getGeometryN( i )->clone() );
                geometries.append( g->serialize() );
                delete g;
            }
            json_geometry.insert( "geometries" , geometries);

        } else {
            qWarning() << QString("Geometry type %1 unknown").arg( QString::fromStdString( this->inner_geometry->getGeometryType() ) );
        }

    } catch ( ... ){
        qWarning() << QString("Geometry type %1 parsing crashed").arg( QString::fromStdString( this->inner_geometry->getGeometryType() ) );
    }

    return json_geometry;*/
}

QString GWSGeometry::toString() const{
    return QString::fromStdString( this->inner_geometry->toString() );
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSPoint GWSGeometry::getRepresentativePoint() const{
    return GWSPoint( this->inner_geometry->getCoordinate()->x , this->inner_geometry->getCoordinate()->y , this->inner_geometry->getCoordinate()->z );
}

bool GWSGeometry::isValid() const{
    return this->inner_geometry ? this->inner_geometry->isValid() : false;
}

bool GWSGeometry::intersects( const GWSGeometry other ) const{
    return this->inner_geometry->intersects( other.inner_geometry );
}

bool GWSGeometry::equals( const GWSGeometry other) const{
    return this->inner_geometry->equals( other.inner_geometry );
}

GWSAreaUnit GWSGeometry::getArea() const{
    return GWSAreaUnit( this->inner_geometry ? this->inner_geometry->getArea() : -1 );
}

GWSLengthUnit GWSGeometry::getDistance( const GWSGeometry other) const{
    return GWSLengthUnit( this->inner_geometry ? this->inner_geometry->distance( other.inner_geometry ) * 110574 : -1 );
}

/**********************************************************************
 SPATIAL TRANSFORMS
**********************************************************************/

GWSPoint GWSGeometry::getCentroid() const{
    if( this && this->inner_geometry ){
        geos::geom::Coordinate centroid;
        this->inner_geometry->getCentroid( centroid );
        return GWSPoint( centroid.x , centroid.y , centroid.z );
    }
    return GWSPoint( NAN , NAN , NAN );
}

GWSGeometry GWSGeometry::getBuffer( double threshold ) const{
    GWSGeometry geom = GWSGeometry( this->inner_geometry->buffer( threshold ) );
    geom.setParent( this->parent() );
    return geom;
}

GWSGeometry GWSGeometry::getUnion(const GWSGeometry other) const{
    GWSGeometry geom = GWSGeometry( this->inner_geometry->Union( other.inner_geometry ) );
    geom.setParent( this->parent() );
    return geom;
}

GWSGeometry GWSGeometry::getIntersection(const GWSGeometry other) const{
    GWSGeometry geom = GWSGeometry( this->inner_geometry->intersection( other.inner_geometry ) );
    geom.setParent( this->parent() );
    return geom;
}
