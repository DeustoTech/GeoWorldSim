#include "Envelope.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>

#include "geos/geom/Envelope.h"

#include "util/geometry/Polygon.h"
#include "util/geometry/GeometryFactory.h"
#include "util/random/UniformDistribution.h"
#include "util/conversors/image_coordinates/ImageCoordinatesConversor.h"

using namespace geos::geom;

QString GWSEnvelope::LEFT_PROP = "left";
QString GWSEnvelope::RIGHT_PROP = "right";
QString GWSEnvelope::BOTTOM_PROP = "bottom";
QString GWSEnvelope::TOP_PROP = "top";

GWSEnvelope::GWSEnvelope() : GWSObject( Q_NULLPTR ){
}

GWSEnvelope::GWSEnvelope(double left, double right, double bottom, double top) : GWSObject( Q_NULLPTR ){
    this->setProperty( LEFT_PROP , left );
    this->setProperty( RIGHT_PROP , right );
    this->setProperty( BOTTOM_PROP , bottom );
    this->setProperty( TOP_PROP , top );
}

GWSEnvelope::GWSEnvelope(const GWSEnvelope &other) : GWSObject( Q_NULLPTR ){
    this->setProperty( LEFT_PROP , other.getMinX() );
    this->setProperty( RIGHT_PROP , other.getMaxX() );
    this->setProperty( BOTTOM_PROP , other.getMinY() );
    this->setProperty( TOP_PROP , other.getMaxY() );
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSEnvelope::toImage(const GWSEnvelope image_bounds, int image_width, int image_height, QColor border_color, int border_width, QColor fill_color) const{
    Q_UNUSED( border_width )

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( border_color );

    QPolygon draw;

    // TOP LEFT
    draw.append( ImageCoordinatesConversor::reprojectPoint(this->getMaxY(),this->getMinX(),image_bounds.getMaxY(),image_bounds.getMinY(),image_bounds.getMaxX(),image_bounds.getMinX(),image_width,image_height ) );

    // TOP RIGHT
    draw.append( ImageCoordinatesConversor::reprojectPoint(this->getMaxY(),this->getMaxX(),image_bounds.getMaxY(),image_bounds.getMinY(),image_bounds.getMaxX(),image_bounds.getMinX(),image_width,image_height ) );

    // BOTTOM RIGHT
    draw.append( ImageCoordinatesConversor::reprojectPoint(this->getMinY(),this->getMaxX(),image_bounds.getMaxY(),image_bounds.getMinY(),image_bounds.getMaxX(),image_bounds.getMinX(),image_width,image_height ) );

    // BOTTOM LEFT
    draw.append( ImageCoordinatesConversor::reprojectPoint(this->getMinY(),this->getMinX(),image_bounds.getMaxY(),image_bounds.getMinY(),image_bounds.getMaxX(),image_bounds.getMinX(),image_width,image_height ) );

    // TOP LEFT
    draw.append( ImageCoordinatesConversor::reprojectPoint(this->getMaxY(),this->getMinX(),image_bounds.getMaxY(),image_bounds.getMinY(),image_bounds.getMaxX(),image_bounds.getMinX(),image_width,image_height ) );

    // Draw borders
    painter.drawPolygon( draw );

    // Draw interior
    QPainterPath path;
    path.addPolygon( draw );
    painter.fillPath( path , QBrush( fill_color ) );*/

    return image;
}

QString GWSEnvelope::toWKT() const{
    return QString("POLYGON ((%1 %2, %3 %2, %3 %4, %1 %4, %1 %2))").arg( this->getMinX() ).arg( this->getMinY() ).arg( this->getMaxX() ).arg( this->getMaxY() );
}

/**********************************************************************
 GETTERS
**********************************************************************/

double GWSEnvelope::getMaxX() const{
    return this->property( RIGHT_PROP ).toDouble();
}

double GWSEnvelope::getMinX() const{
    return this->property( LEFT_PROP ).toDouble();
}

double GWSEnvelope::getMaxY() const{
    return this->property( TOP_PROP ).toDouble();
}

double GWSEnvelope::getMinY() const{
    return this->property( BOTTOM_PROP ).toDouble();
}

GWSAreaUnit GWSEnvelope::getArea() const{
    return GWSAreaUnit( geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() ).getArea() );
}

GWSCoordinate GWSEnvelope::getCentroid() const{
    geos::geom::Coordinate c;
    geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() ).centre( c );
    return GWSCoordinate( c.x , c.y , c.z );
}

GWSCoordinate GWSEnvelope::getTopLeft() const{
    return GWSCoordinate( this->getMinX() , this->getMaxY() );
}

GWSCoordinate GWSEnvelope::getBottomLeft() const{
    return GWSCoordinate( this->getMinX() , this->getMinY() );
}

GWSCoordinate GWSEnvelope::getTopRight() const{
    return GWSCoordinate( this->getMaxX() , this->getMaxY() );
}

GWSCoordinate GWSEnvelope::getBottomRight() const{
    return GWSCoordinate( this->getMaxX() , this->getMinY() );
}

GWSCoordinate GWSEnvelope::getRandomCoordinate() const{
    double x_width = qAbs( this->getMaxX() - this->getMinX() );
    double y_width = qAbs( this->getMaxY() - this->getMinY() );
    double x = UniformDistribution::uniformDistribution() * x_width;
    double y = UniformDistribution::uniformDistribution() * y_width;
    return GWSCoordinate( this->getMinX() + x , this->getMinY() + y );
}

bool GWSEnvelope::covers(GWSEnvelope env) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    return copy.covers( geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() ) );
}

bool GWSEnvelope::covers(GWSCoordinate coor) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    return copy.covers( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) );
}

bool GWSEnvelope::contains(GWSCoordinate coor) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    return copy.contains( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) );
}

bool GWSEnvelope::contains(GWSEnvelope env) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    return copy.contains( geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() ) );
}

bool GWSEnvelope::intersects(GWSEnvelope env) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    return copy.intersects( geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() ) );
}

void GWSEnvelope::expandToInclude(GWSCoordinate coor){
    geos::geom::Envelope envelope = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    envelope.expandToInclude( coor.getX() , coor.getY() );
    this->setProperty( LEFT_PROP , envelope.getMinX() );
    this->setProperty( RIGHT_PROP , envelope.getMaxX() );
    this->setProperty( BOTTOM_PROP , envelope.getMinY() );
    this->setProperty( TOP_PROP , envelope.getMaxY() );
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSEnvelope GWSEnvelope::createBuffer(double buffer) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    copy.expandBy( buffer , buffer );
    return GWSEnvelope( copy.getMinX() , copy.getMaxX() , copy.getMinY() , copy.getMaxY() );
}

GWSEnvelope GWSEnvelope::createUnion(GWSEnvelope other) const{
    geos::geom::Envelope copy = geos::geom::Envelope( this->getMinX() , this->getMaxX() , this->getMinY() , this->getMaxY() );
    copy.expandToInclude( other.getMaxX() );
    copy.expandToInclude( other.getMaxY() );
    copy.expandToInclude( other.getMinX() );
    copy.expandToInclude( other.getMinY() );
    return GWSEnvelope( copy.getMinX() , copy.getMaxX() , copy.getMinY() , copy.getMaxY() );
}

GWSPolygon* GWSEnvelope::createPolygon() const{
    QList<GWSCoordinate> coors;
    coors << this->getTopLeft() << this->getTopRight() << this->getBottomRight() << this->getBottomLeft() << this->getTopLeft();
    return GWSGeometryFactory::globalInstance()->createPolygon( coors );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

GWSEnvelope& GWSEnvelope::operator= (const GWSEnvelope& other){
    this->setProperty( LEFT_PROP , other.getMinX() );
    this->setProperty( RIGHT_PROP , other.getMaxX() );
    this->setProperty( BOTTOM_PROP , other.getMinY() );
    this->setProperty( TOP_PROP , other.getMaxY() );
    return *this;
}

bool GWSEnvelope::operator == (const GWSEnvelope& other) const{
    return this->getMinX() == other.getMinX() &&
            this->getMaxX() == other.getMaxX() &&
            this->getMinY() == other.getMinY() &&
            this->getMaxY() == other.getMaxY();
}

bool GWSEnvelope::operator != (const GWSEnvelope& other) const{
    return !(*this == other);
}


