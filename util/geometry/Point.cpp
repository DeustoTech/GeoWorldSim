#include "Point.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>

#include "geos/geom/GeometryFactory.h"
#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSPoint::GWSPoint(geos::geom::Point *geom, QObject *parent) :  GWSGeometry( geom , parent) {
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSPoint::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color, QImage icon) const{
    Q_UNUSED(fill_color);

    // Image to be returned
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    if( !this->inner_geometry ){ return image; }

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    QPen pen( border_color );
    pen.setWidth( image_width / 50 );
    painter.setPen( pen );

    const geos::geom::Point* point = dynamic_cast<const geos::geom::Point*>( this->inner_geometry );

    if( !point->getCoordinate()->isNull() ){

        QPoint draw = ImageCoordinatesConversor::reprojectPoint(
                    point->getCoordinate()->y,
                    point->getCoordinate()->x,
                    image_bounds.getMaxY(),
                    image_bounds.getMinY(),
                    image_bounds.getMaxX(),
                    image_bounds.getMinX(),
                    image_width,
                    image_height );

        if( !icon.isNull() ){

            int w = image_width / 10;
            int h = image_height / 10;
            draw.setX( draw.x() - w/2 );
            draw.setY( draw.y() - h );
            painter.drawImage( draw , icon.scaled( w , h ) );

        } else {

            painter.drawPoint( draw );
        }
    }*/
    return image;
}

QJsonObject GWSPoint::toGeoJSON(QJsonObject properties) const{

    // Geometry
    QJsonObject json_geometry;
    json_geometry.insert( "properties" , properties );

    if( !this->inner_geometry ){ return json_geometry; }

    if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_POINT ){

        geos::geom::Point* point = dynamic_cast<geos::geom::Point*>( this->inner_geometry->clone() );

        json_geometry.insert("type", QString("Point") );

        QJsonArray coordinates;
        const geos::geom::Coordinate* coor = point->getCoordinate();
        if( coor ){
            coordinates.append( coor->x );
            coordinates.append( coor->y );
            coordinates.append( coor->z );
        }
        json_geometry.insert("coordinates", coordinates);

        delete point;
        return json_geometry;
    }

    return GWSGeometry::toGeoJSON( properties );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSPoint::move(double move_x, double move_y){

    // Get previous location
    const geos::geom::Coordinate* c = this->inner_geometry->getCoordinate();
    geos::geom::Coordinate coor( c->x + move_x , c->y + move_y , c->z );
    const geos::geom::Geometry* temp = this->inner_geometry;

    // Create new location and delete previous
    this->inner_geometry = temp->getFactory()->createPoint( coor );
    delete temp;
}

/**********************************************************************
 METHODS
**********************************************************************/
