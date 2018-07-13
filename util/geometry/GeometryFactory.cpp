#include "GeometryFactory.h"

#include <QDebug>

#include "geos/geom/PrecisionModel.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Point.h"
#include "geos/io/WKTReader.h"
#include "geos/io/WKTWriter.h"

#include "../../util/random/UniformDistribution.h"

GWSGeometryFactory* GWSGeometryFactory::globalInstance(){
    static GWSGeometryFactory instance;
    return &instance;
}

GWSGeometryFactory::GWSGeometryFactory() : GWSObject( Q_NULLPTR ){
    // Create geometry factory
    this->geometry_factory = geos::geom::GeometryFactory::getDefaultInstance();
}

GWSGeometryFactory::~GWSGeometryFactory(){
    //delete this->geometry_factory;
}

/**********************************************************************
 GETTERS
**********************************************************************/

const GWSGrid GWSGeometryFactory::getElevationModel() const{
    return this->elevation_model;
}

/*const geos::geom::GeometryFactory* GWSGeometryFactory::getGeometryFactory() const{
    return this->geometry_factory;
}*/

GWSCoordinate GWSGeometryFactory::getRandomPoint( const GWSGeometry *bounds, unsigned int seed) const{
    if( bounds->getCoordinates().size() <= 2 ){
        return bounds->getRepresentativeCoordinate();
    }

    srand( seed );

    int max_attemps = 4;
    do {

        GWSCoordinate center = bounds->getCentroid();
        /*double x_diff = bounds->getEnvelope().getMaxX() - bounds->getEnvelope().getMinX();
        double y_diff = bounds->getEnvelope().getMaxY() - bounds->getEnvelope().getMinY();
        double x_incr = (( qrand() * 100000 ) % int(x_diff * 100000) ) / 100000;
        double y_incr = (( qrand() * 100000 ) % int(y_diff * 100000) ) / 100000;

        if( bounds->inner_geometry->getEnvelopeInternal() && bounds->inner_geometry->getEnvelopeInternal()->contains( bounds->getEnvelope().getMinX() + x_incr , bounds->getEnvelope().getMinY() + y_incr ) ){
            return GSSCoordinate( center.getX() + x_incr , center.getY() + y_incr , center.getZ() );
        }*/
        return center;

        max_attemps--;
    } while( max_attemps > 0);

    return bounds->getRepresentativeCoordinate();
}

/**********************************************************************
 CREATORS
**********************************************************************/

GWSGeometry* GWSGeometryFactory::createGeometry(QString wkt , bool elevate) const{

    try {
        // Create reader to build geometry without our elevation
        geos::io::WKTReader reader = geos::io::WKTReader( this->geometry_factory );
        geos::geom::Geometry* g = reader.read( wkt.trimmed().toStdString() );

        // POINT
        if( g->getGeometryTypeId() == geos::geom::GEOS_POINT ){
            return this->createPoint( dynamic_cast<geos::geom::Point*>( g ) , elevate );
        }

        // LINESTRING
        if( g->getGeometryTypeId() == geos::geom::GEOS_LINESTRING ){
            return this->createLineString( dynamic_cast<geos::geom::LineString*>( g ) , elevate );
        }

        // POLYGON
        if( g->getGeometryTypeId() == geos::geom::GEOS_POLYGON ){
            return this->createPolygon( dynamic_cast<Polygon*>( g ) , elevate );
        }

        return new GWSGeometry( g );
    } catch(...){
    }
    return 0;
}

QString GWSGeometryFactory::toWKT(GWSGeometry *geom) const{
    if( !geom ){ return ""; }
    geos::io::WKTWriter writer;
    return QString::fromStdString( writer.write( geom->inner_geometry ) );
}

// POINT

GWSPoint* GWSGeometryFactory::createPoint( geos::geom::Point* gpoint, bool elevate) const{
    if( !elevate ){
        return new GWSPoint( gpoint );
    }
    double x = gpoint->getCoordinate()->x;
    double y = gpoint->getCoordinate()->y;
    double z = gpoint->getCoordinate()->z;
    delete gpoint;
    return this->createPoint( x , y , z , elevate );
}


GWSPoint* GWSGeometryFactory::createPoint(double x, double y, double z, bool elevate) const{
    return this->createPoint( GWSCoordinate( x , y , z ) , elevate );
}

GWSPoint* GWSGeometryFactory::createPoint(GWSCoordinate c , bool elevate) const{
    return new GWSPoint( this->geometry_factory->createPoint( geos::geom::Coordinate( c.getX() , c.getY() , elevate ? this->getElevation( c ) : c.getZ() ) ) );
}

// LINESTRING

GWSLineString* GWSGeometryFactory::createLineString( geos::geom::LineString *gline, bool elevate) const{
    if( !elevate ){
        return new GWSLineString( gline );
    }
    QList<GWSCoordinate> coors;
    for(unsigned int i = 0; i < gline->getCoordinates()->size(); i++){
        coors.append( GWSCoordinate( gline->getCoordinateN( i ).x , gline->getCoordinateN( i ).y ) );
    }
    delete gline;
    return this->createLineString( coors , elevate );
}

GWSLineString* GWSGeometryFactory::createLineString(GWSCoordinate c1, GWSCoordinate c2 , bool elevate) const{
    QList<GWSCoordinate> coors;
    coors.append( c1 );
    coors.append( c2 );
    return this->createLineString( coors , elevate );
}

GWSLineString* GWSGeometryFactory::createLineString(QList<GWSCoordinate> coors , bool elevate) const{
    if( coors.isEmpty() || coors.size() == 1){
        qWarning() << "Empty or single coordinate linestring";
        return 0;
    }
    CoordinateSequence* seq = this->geometry_factory->getCoordinateSequenceFactory()->create( (std::size_t)0 );
    foreach(GWSCoordinate c , coors){
        seq->add( geos::geom::Coordinate( c.getX() , c.getY() , elevate ? this->getElevation( c ) : c.getZ() ) );
    }
    return new GWSLineString( this->geometry_factory->createLineString( seq ) );
}

// POLYGON

GWSPolygon* GWSGeometryFactory::createPolygon( geos::geom::Polygon *gpolygon, bool elevate) const{
    if( !elevate ){
        return new GWSPolygon( gpolygon );
    }

    QList<QList<GWSCoordinate>> rings_coors;
    const geos::geom::LineString* out_ring = gpolygon->getExteriorRing();

    QList<GWSCoordinate> out_coors;
    for(unsigned int i = 0 ; i < out_ring->getCoordinates()->size() ; i++){
        out_coors.append( GWSCoordinate( out_ring->getCoordinateN( i ).x , out_ring->getCoordinateN( i ).y , out_ring->getCoordinateN( i ).z ) );
    }
    rings_coors.append( out_coors );

    for(unsigned int i = 0; i < gpolygon->getNumInteriorRing(); i++){
        const geos::geom::LineString* in_ring = gpolygon->getInteriorRingN( i );

        QList<GWSCoordinate> in_coors;
        for(unsigned int i = 0 ; i < in_ring->getCoordinates()->size() ; i++){
            in_coors.append( GWSCoordinate( in_ring->getCoordinateN( i ).x , in_ring->getCoordinateN( i ).y , in_ring->getCoordinateN( i ).z ) );
        }
        rings_coors.append( in_coors );
    }
    delete gpolygon;
    return this->createPolygon( rings_coors , elevate );
}

GWSPolygon* GWSGeometryFactory::createPolygon(QList<QList<GWSCoordinate> > rings_coors , bool elevate) const{
    LinearRing* shell = 0;
    std::vector<geos::geom::Geometry*> holes;

    foreach(QList<GWSCoordinate> ring_coors , rings_coors){
        CoordinateSequence* coordinates = this->geometry_factory->getCoordinateSequenceFactory()->create( (std::size_t)0 );
        foreach(GWSCoordinate c , ring_coors){
            coordinates->add( geos::geom::Coordinate( c.getX() , c.getY() , elevate ? this->getElevation( c ) : c.getZ() ) );
        }

        // First Coordinate list is the outer ring/shell
        if( !shell ){
            shell = this->geometry_factory->createLinearRing( coordinates );
        } else {
            LinearRing* r = this->geometry_factory->createLinearRing( coordinates );
            holes.push_back( r );
        }
    }
    return new GWSPolygon( this->geometry_factory->createPolygon( *shell , holes ) );
}

GWSPolygon* GWSGeometryFactory::createPolygon( QList<GWSCoordinate> outer_coors , bool elevate) const{
    QList< QList< GWSCoordinate > > rings;
    rings.append( outer_coors );
    return this->createPolygon( rings , elevate );
}

/**********************************************************************
 ELEVATION
**********************************************************************/

double GWSGeometryFactory::getElevation( GWSCoordinate coor) const{
    return this->elevation_model.getCellValue( coor );
}

void GWSGeometryFactory::setElevationPoint( GWSCoordinate coor, double value ){
    this->elevation_model.setCellValue( coor , value );
}

void GWSGeometryFactory::setElevationModel( const GWSGrid& elevation){
    this->elevation_model = elevation;
}
