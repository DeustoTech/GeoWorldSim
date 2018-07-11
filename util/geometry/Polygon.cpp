#include "Polygon.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>
#include "geos/geom/LineString.h"

#include "util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSPolygon::GWSPolygon( geos::geom::Polygon *geom, QObject *parent) : GWSGeometry( geom , parent ){

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSPolygon::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color, QImage icon) const{
    Q_UNUSED(icon);

    // If bounds are too big, just return coordinate
    if( (this->getArea() * 25) > image_bounds.getArea() ){
        return GWSGeometry::toImage( image_bounds , image_width , image_height , fill_color , border_width , fill_color , icon );
    }

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    if( !this->inner_geometry ){ return image; }

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    QPen pen;
    pen.setColor( border_color );
    pen.setWidth( border_width );
    painter.setPen( pen );

    const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>( this->inner_geometry );

    QPolygon draw;
    for( unsigned int i = 0; i < polygon->getExteriorRing()->getCoordinates()->size(); i++ ){

        if( !polygon->getExteriorRing()->getCoordinates()->getAt(i).isNull() ){
            draw.append( ImageCoordinatesConversor::reprojectPoint(
                             polygon->getExteriorRing()->getCoordinateN(i).y,
                             polygon->getExteriorRing()->getCoordinateN(i).x,
                             image_bounds.getMaxY(),
                             image_bounds.getMinY(),
                             image_bounds.getMinX(),
                             image_bounds.getMaxX(),
                             image_width,
                             image_height ) );
        }
    }

    // Draw borders
    painter.drawPolygon( draw );

    // Draw interior
    QPainterPath path;
    path.addPolygon( draw );
    QBrush brush;
    brush.setColor( fill_color );
    brush.setStyle( Qt::SolidPattern );
    painter.fillPath( path , brush );*/

    return image;
}


QJsonObject GWSPolygon::toGeoJSON(QJsonObject properties) const{

    // Geometry
    QJsonObject json_geometry;
    json_geometry.insert( "properties" , properties );

    if( !this->inner_geometry ){ return json_geometry; }

    if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_POLYGON ) {
        geos::geom::Polygon* polygon = dynamic_cast<geos::geom::Polygon*>( this->inner_geometry->clone() );

        json_geometry.insert("type", QString("Polygon") );

        QJsonArray rings;

        QJsonArray ringCoordinates;
        for(unsigned int i = 0; i < polygon->getExteriorRing()->getCoordinates()->size(); i++){
            QJsonArray coordinatePair;
            coordinatePair.append( polygon->getExteriorRing()->getCoordinates()->getAt( i ).x );
            coordinatePair.append( polygon->getExteriorRing()->getCoordinates()->getAt( i ).y );
            coordinatePair.append( polygon->getExteriorRing()->getCoordinates()->getAt( i ).z );
            ringCoordinates.append(coordinatePair);
        }
        rings.append(ringCoordinates);

        for(unsigned int i = 0; i < polygon->getNumInteriorRing(); i++){
            QJsonArray ringCoordinates;
            for(unsigned int j = 0; j < polygon->getInteriorRingN( i )->getCoordinates()->size(); j++){
                QJsonArray coordinatePair;
                coordinatePair.append( polygon->getInteriorRingN( i )->getCoordinates()->getAt( j ).x );
                coordinatePair.append( polygon->getInteriorRingN( i )->getCoordinates()->getAt( j ).y );
                coordinatePair.append( polygon->getInteriorRingN( i )->getCoordinates()->getAt( j ).z );
                ringCoordinates.append(coordinatePair);
            }
            rings.append(ringCoordinates);
        }

        json_geometry.insert("coordinates", rings);
        delete polygon;
        return json_geometry;
    }

    return GWSGeometry::toGeoJSON( properties );
}


/**********************************************************************
 METHODS
**********************************************************************/
