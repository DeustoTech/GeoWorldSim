#include "Voronoi.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"
#include "geos/triangulate/VoronoiDiagramBuilder.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateSequenceFactory.h"

Voronoi::Voronoi(QObject *parent) : QObject(parent){

}

/**********************************************************************
 METHODS
**********************************************************************/

QList<GWSGeometry*> Voronoi::createVoronoi(QList<GWSCoordinate> coordinates, const GWSEnvelope envelope, const GWSGeometryFactory *geometry_controller){

    QList<GWSGeometry*> voronoi_geoms;
    geos::triangulate::VoronoiDiagramBuilder* builder = new geos::triangulate::VoronoiDiagramBuilder();

    // Set envelope
    geos::geom::Envelope* env = new geos::geom::Envelope( envelope.getMinX() , envelope.getMaxX() , envelope.getMinY() , envelope.getMaxY() );
    builder->setClipEnvelope( env );

    // Set points
    geos::geom::CoordinateSequence* seq = geometry_controller->getGeometryFactory()->getCoordinateSequenceFactory()->create();
    foreach (GWSCoordinate c , coordinates) {
        seq->add( geos::geom::Coordinate( c.getX() , c.getY() , c.getZ() ) );
    }
    builder->setSites( *seq );

    // Execute voronoi
    std::auto_ptr<geos::geom::GeometryCollection> result = builder->getDiagram( *geometry_controller->getGeometryFactory() );

    geos::geom::GeometryCollection* collection = result.release();
    for(unsigned int x = 0; x < collection->getNumGeometries(); x++ ){
        GWSPolygon* polygon = dynamic_cast<GWSPolygon*>( geometry_controller->createGeometry( QString::fromStdString( collection->getGeometryN( x )->toString() ) ) );
        voronoi_geoms.append( polygon );
    }

    delete builder;
    delete env;
    delete seq;

    return voronoi_geoms;
}

