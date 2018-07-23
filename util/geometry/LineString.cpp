#include "LineString.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSLineString::GWSLineString(QList<GWSPoint> points ) : GWSGeometry(){
    geos::geom::CoordinateSequence* seq = geos::geom::GeometryFactory::getDefaultInstance()->getCoordinateSequenceFactory()->create();
    foreach( GWSPoint p , points ){
           seq->add( geos::geom::Coordinate( p.getX() , p.getY() , p.getZ() ) );
    }
    this->inner_geometry = geos::geom::GeometryFactory::getDefaultInstance()->createLineString( seq );
}

GWSLineString::GWSLineString(LineString *inner_geometry) : GWSGeometry(){
    this->inner_geometry = inner_geometry;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSLineString::deserialize(QJsonObject json){

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSLineString::serialize() const{

    QJsonObject json_geometry;
    if( !this->inner_geometry ){ return json_geometry; }

    if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_LINESTRING ){

        geos::geom::LineString* line = dynamic_cast<geos::geom::LineString*>( this->inner_geometry->clone() );
        json_geometry.insert("type", QString("LineString") );

        QJsonArray lineCoordinates;

        for(unsigned int i = 0; i < line->getCoordinates()->size(); i++){
            if( !line->getCoordinates()->getAt(i).isNull() ){
                QJsonArray coordinatePair;
                coordinatePair.append( line->getCoordinates()->getAt(i).x );
                coordinatePair.append( line->getCoordinates()->getAt(i).y );
                coordinatePair.append( line->getCoordinates()->getAt(i).z );
                lineCoordinates.append(coordinatePair);
            }
        }
        json_geometry.insert("coordinates", lineCoordinates);

        delete line;
        return json_geometry;
    }
    return GWSGeometry::serialize();
}

/*QImage GWSLineString::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color, QImage icon) const{
    Q_UNUSED(fill_color);
    Q_UNUSED(icon);

    // If bounds are too big, just return coordinate
    if( image_bounds.getArea() > (this->getEnvelope().getArea() * 100) ){
        return GWSGeometry::toImage( image_bounds , image_width , image_height , fill_color , border_width , fill_color , icon );
    }

    // Image to be returned
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

    if( !this->inner_geometry ){ return image; }

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    QPen pen( border_color );
    pen.setWidth( image_width / 50 );
    painter.setPen( pen );

    const geos::geom::LineString* line = dynamic_cast<const geos::geom::LineString*>( this->inner_geometry );

    // If bounds are too big, skip some coordinates
    unsigned int incr = 1;
    if( image_bounds.getArea().number() > this->getLength().number() * 5 ){ incr = 2; }
    if( image_bounds.getArea().number() > this->getLength().number() * 50 ){ incr = 5; }

    for( unsigned int i = 0; i < line->getCoordinates()->size()-1; i+=incr){

        QLine draw;
        if( !line->getCoordinateN(i).isNull() && !line->getCoordinateN(i+1).isNull() ){
            draw = QLine(
                        ImageCoordinatesConversor::reprojectPoint(
                            line->getCoordinateN(i).y,
                            line->getCoordinateN(i).x,
                            image_bounds.getMaxY(),
                            image_bounds.getMinY(),
                            image_bounds.getMaxX(),
                            image_bounds.getMinX(),
                            image_width,
                            image_height ) ,
                        ImageCoordinatesConversor::reprojectPoint(
                            line->getCoordinateN(i+1).y,
                            line->getCoordinateN(i+1).x,
                            image_bounds.getMaxY(),
                            image_bounds.getMinY(),
                            image_bounds.getMaxX(),
                            image_bounds.getMinX(),
                            image_width,
                            image_height ) );
        }

        // Draw line
        painter.drawLine( draw );
    }

    return image;
}*/

/**********************************************************************
 GETTERS
**********************************************************************/

GWSLengthUnit GWSLineString::getLength() const{
    return GWSLengthUnit( dynamic_cast<const geos::geom::LineString*>( this->inner_geometry )->getLength() );
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSLineString GWSLineString::getReversed() const{
    const geos::geom::LineString* l = dynamic_cast<const geos::geom::LineString*>( this->inner_geometry );
    return GWSLineString( dynamic_cast<geos::geom::LineString*>( l->reverse() ) );
}

/*QList<GWSLineString*> GWSLineString::createChopsFromCoordinates() const{
    QList<GWSLineString*> lines;
    QList<GWSCoordinate> coors = this->getCoordinates();
    for( int i = 0 ; i < coors.size()-1 ; i++ ){
        lines.append( GWSGeometryFactory::globalInstance()->createLineString( coors.at(i) , coors.at(i+1) ) );
    }
    return lines;
}

QList<GWSLineString*> GWSLineString::createAmountChops(int amount) const{
    QList<GWSLineString*> lines;
    GWSCoordinate start = this->getFirstCoordinate();
    GWSCoordinate end = this->getLastCoordinate();
    double xmid = ( start.getX() + end.getX() )/2;
    double ymid = ( start.getY() + end.getY() )/2;
    lines.append( GWSGeometryFactory::globalInstance()->createLineString( start , GWSCoordinate( xmid , ymid ) ) );
    lines.append( GWSGeometryFactory::globalInstance()->createLineString( GWSCoordinate( xmid , ymid ) , end ) );
    return lines;
}

GWSLineString* GWSLineString::createCopy() const{
    return dynamic_cast<GWSLineString*>( GWSGeometry::createCopy() );
}
*/
