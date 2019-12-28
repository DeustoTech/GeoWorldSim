#include "ElevationServiceAreaCalculator.h"

ElevationServiceAreaCalculator::ElevationServiceAreaCalculator(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GSSGeometry* ElevationServiceAreaCalculator::getNeighbouringHigherCells(GSSCoordinate coord, GSSGrid grid, GSSGeometryController* controller){
    QList<GSSEnvelope> visited_cells;
    QList<GSSEnvelope> surrounding_cells = grid.getSurroundingCells( coord );
    return ElevationServiceAreaCalculator::getNeighbouringHigherCellsRecursive( coord, grid, controller, surrounding_cells, &visited_cells );
}

/**********************************************************************
 PRIVATE
**********************************************************************/

GSSGeometry* ElevationServiceAreaCalculator::getNeighbouringHigherCellsRecursive(GSSCoordinate coord, GSSGrid grid, GSSGeometryController* controller, QList<GSSEnvelope> surrounding_cells, QList<GSSEnvelope> *visited_cells ){

    double reference_elevation = controller->getElevation( coord  );

    std::vector< GSSGeometry*> polygons;
    GSSGeometry* final_polygon = 0;

    foreach ( GSSEnvelope envelope, surrounding_cells ){
        GSSCoordinate envelope_centroid = envelope.getCentroid();

        if( controller->getElevation( envelope_centroid ) >= reference_elevation && !visited_cells->contains( envelope ) ){

            visited_cells->append( envelope );

            QList<GSSCoordinate> seq;
            seq.append( GSSCoordinate ( envelope.getMinX(), envelope.getMaxY() ) );
            seq.append( GSSCoordinate ( envelope.getMaxX(), envelope.getMaxY() ) );
            seq.append( GSSCoordinate ( envelope.getMaxX(), envelope.getMinY() ) );
            seq.append( GSSCoordinate ( envelope.getMinX(), envelope.getMinY() ) );
            seq.append( GSSCoordinate ( envelope.getMinX(), envelope.getMaxY() ) );

            // Create basin polygon
            QList< QList<GSSCoordinate> > linestring;
            linestring.append( seq );

            GSSPolygon* envelope_polygon = controller->createPolygon( linestring );
            polygons.push_back( envelope_polygon );
            GSSGeometry* p = ElevationServiceAreaCalculator::getNeighbouringHigherCellsRecursive( envelope_centroid, grid, controller, grid.getSurroundingCells( envelope_centroid ), visited_cells );

            if ( p ){
                polygons.push_back( p );
            }
        }
    }

    if( !polygons.empty() ){

        foreach( GSSGeometry* polyg, polygons ){
            if( !final_polygon ) {
                final_polygon = polyg;
            } else {
                final_polygon = final_polygon->createUnion( polyg );
            }
        }
        return final_polygon;
    }
    return 0;
}
