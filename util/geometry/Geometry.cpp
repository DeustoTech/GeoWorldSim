#include "Geometry.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>
#include <QDebug>

#include "../../agent/Agent.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/CoordinateSequenceFilter.h"

#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"

//#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSGeometry::GWSGeometry( QObject* parent ) : GWSObject( parent ){
}

GWSGeometry::GWSGeometry( geos::geom::Geometry* inner_geometry ) : GWSObject( Q_NULLPTR ){
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

    if( this->inner_geometry ){ delete this->inner_geometry; }

    GWSObject::deserialize( json );
    QString geom_type = json.value("type").toString();
    const GeometryFactory* factory = geos::geom::GeometryFactory::getDefaultInstance();
    QJsonArray coors = json.value("coordinates").toArray();

    if( geom_type.toLower() == "point" ){
        this->inner_geometry = factory->createPoint(
                    geos::geom::Coordinate(
                        coors.size() > 0 ? coors.at(0).toDouble() : 0 ,
                        coors.size() > 1 ? coors.at(1).toDouble() : 0 ,
                        coors.size() > 2 ? coors.at(2).toDouble() : 0 )
                    );
    } else if ( geom_type.toLower() == "linestring" ){

        geos::geom::CoordinateSequence* seq = factory->getCoordinateSequenceFactory()->create();

        for( int i = 0 ; i < coors.size() ; i++ ){
            QJsonArray coor = coors.at( i ).toArray();
            seq->add( geos::geom::Coordinate(
                          coor.size() > 0 ? coor.at(0).toDouble() : 0 ,
                          coor.size() > 1 ? coor.at(1).toDouble() : 0 ,
                          coor.size() > 2 ? coor.at(2).toDouble() : 0 )
                      );
        }
        this->inner_geometry = factory->createLineString( seq );
    } else if ( geom_type.toLower() == "polygon" ){

        geos::geom::LinearRing* outer_ring = Q_NULLPTR;
        std::vector<geos::geom::Geometry *>* holes = new std::vector<Geometry*>();

        for( int i = 0 ; i < coors.size() ; i++ ){

            QJsonArray ring = coors.at(i).toArray();
            geos::geom::CoordinateSequence* seq = factory->getCoordinateSequenceFactory()->create();

            for( int j = 0 ; j < ring.size() ; j++ ){
                QJsonArray coor = ring.at( j ).toArray();
                seq->add( geos::geom::Coordinate(
                              coor.size() > 0 ? coor.at(0).toDouble() : 0 ,
                              coor.size() > 1 ? coor.at(1).toDouble() : 0 ,
                              coor.size() > 2 ? coor.at(2).toDouble() : 0 )
                          );
            }

            if( i == 0 ){
                outer_ring = factory->createLinearRing( seq );
            } else {
                holes->push_back( factory->createLinearRing( seq ) );
            }
        }
        this->inner_geometry = factory->createPolygon( outer_ring , holes );
    }

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

    QJsonObject json_geometry = GWSObject::serialize();
    if( !this->inner_geometry ){ return json_geometry; }

    try {

        // POINT
        if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_POINT ){
                json_geometry.insert("type", QString("Point") );

                QJsonArray coordinates;
                const geos::geom::Coordinate* coor = this->inner_geometry->getCoordinate();
                if( coor ){
                    coordinates.append( coor->x );
                    coordinates.append( coor->y );
                    coordinates.append( coor->z );
                }
                json_geometry.insert("coordinates", coordinates);
                return json_geometry;
        }

        // LINESTRING
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

        // POLYGON
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


bool GWSGeometry::isGeometryValid() const{
    return this->inner_geometry ? this->inner_geometry->isValid() : false;
}

GWSAreaUnit GWSGeometry::getArea() const{
    return GWSAreaUnit( this->inner_geometry ? this->inner_geometry->getArea() : -1 );
}

double GWSGeometry::getGeometryMaxX() const{
    if( this->inner_geometry ){
        return this->inner_geometry->getEnvelopeInternal()->getMaxX();
    }
}

double GWSGeometry::getGeometryMinX() const{
    if( this->inner_geometry ){
        return this->inner_geometry->getEnvelopeInternal()->getMinX();
    }
}

double GWSGeometry::getGeometryMaxY() const{
    if( this->inner_geometry ){
        return this->inner_geometry->getEnvelopeInternal()->getMaxY();
    }
}

double GWSGeometry::getGeometryMinY() const{
    if( this->inner_geometry ){
        return this->inner_geometry->getEnvelopeInternal()->getMinY();
    }
}

GWSCoordinate GWSGeometry::getCentroid() const{
    if( this && this->inner_geometry ){
        geos::geom::Coordinate centroid;
        this->inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    return GWSCoordinate();
}

/**********************************************************************
 SPATIAL COMPARATORS
**********************************************************************/

bool GWSGeometry::isInBounds(double minX, double maxX, double minY, double maxY) const{
    return this->inner_geometry ?
                geos::geom::Envelope( minX , maxX , minY , maxY ).contains( *this->inner_geometry->getEnvelopeInternal() )
              : false;
}

bool GWSGeometry::intersects( const GWSGeometry* other ) const{
    return this->inner_geometry ? this->inner_geometry->intersects( other->inner_geometry ) : false;
}

bool GWSGeometry::intersects( double minX, double maxX, double minY, double maxY) const{
    return this->inner_geometry ?
                this->inner_geometry->getEnvelopeInternal()->intersects( geos::geom::Envelope( minX , maxX , minY , maxY ) )
                : false;
}

bool GWSGeometry::equals( const GWSGeometry* other) const{
    return this->inner_geometry ? this->inner_geometry->equals( other->inner_geometry ) : false;
}

GWSLengthUnit GWSGeometry::getDistance( const GWSGeometry* other) const{
    return GWSLengthUnit( this->inner_geometry ? this->inner_geometry->distance( other->inner_geometry )  * 110574 : -1 );
}

/**********************************************************************
 SPATIAL TRANSFORMS
**********************************************************************/

void GWSGeometry::transformMove(GWSCoordinate apply_movement){
    if( !apply_movement.isValid() ){ return; }

    if( !this->inner_geometry ){
        this->inner_geometry = geos::geom::GeometryFactory::getDefaultInstance()->createPoint(
                    geos::geom::Coordinate( apply_movement.getX() , apply_movement.getY() , apply_movement.getZ()
                    ) );
        return;
    }
    // Else
    TransformMoveFilter move = TransformMoveFilter( apply_movement );
    this->inner_geometry->apply_rw( move );
}

void GWSGeometry::transformBuffer( double threshold ){
    geos::geom::Geometry* buffered = this->inner_geometry->buffer( threshold );
    delete this->inner_geometry;
    this->inner_geometry = buffered;
}

void GWSGeometry::transformUnion( const GWSGeometry* other){
    geos::geom::Geometry* unioned = this->inner_geometry->Union( other->inner_geometry );
    delete this->inner_geometry;
    this->inner_geometry = unioned;
}

void GWSGeometry::transformIntersection(const GWSGeometry* other){
    geos::geom::Geometry* intersected = this->inner_geometry->intersection( other->inner_geometry );
    delete this->inner_geometry;
    this->inner_geometry = intersected;
}

/**********************************************************************
 SPATIAL TRANSFORMS FILTERS
**********************************************************************/

TransformMoveFilter::TransformMoveFilter( GWSCoordinate move ) : geos::geom::CoordinateSequenceFilter() {
    this->apply_movement = move;
}

void TransformMoveFilter::filter_rw(CoordinateSequence&  seq , std::size_t i ){
    const geos::geom::Coordinate origin = seq.getAt(i);
    geos::geom::Coordinate moved( origin.x + this->apply_movement.getX() , origin.y + this->apply_movement.getY() , origin.z + this->apply_movement.getZ() );
    seq.setAt(moved, i);
    this->moved_coor_indexes.append( i );
    if( this->moved_coor_indexes.size() == seq.size() ){ this->finished = true; }
}
void TransformMoveFilter::filter_ro(CoordinateSequence &seq, std::size_t i){}
bool TransformMoveFilter::isDone() const { return this->finished; }
bool TransformMoveFilter::isGeometryChanged() const { return true; }
