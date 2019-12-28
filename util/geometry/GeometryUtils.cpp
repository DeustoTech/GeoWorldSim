#include "GeometryUtils.h"

#include <QDebug>

#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Polygon.h"
#include "geos/simplify/DouglasPeuckerSimplifier.h"
#include "../../util/conversors/projection/ProjectionConversor.h"

GWSGeometryUtils* GWSGeometryUtils::globalInstance(){
    static GWSGeometryUtils instance;
    return &instance;
}

GWSGeometryUtils::GWSGeometryUtils(QObject *parent) : QObject(parent){
}

GWSGeometryUtils::~GWSGeometryUtils(){
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSGeometry* GWSGeometryUtils::simplifyDouglasPeucker(GWSGeometry* geom, int distance_tolerance){
    geos::simplify::DouglasPeuckerSimplifier* simplifier = new geos::simplify::DouglasPeuckerSimplifier( geom->inner_geometry );
    simplifier->setDistanceTolerance( distance_tolerance );
    return new GWSGeometry( simplifier->getResultGeometry().release() );
}

GWSGeometry* GWSGeometryUtils::reprojectGeometry(const GWSGeometry* geom, const GWSGeometryFactory* controller, int from_epsg_code , int to_epsg_code){

    try {
        // POINT
        /*if( geom->inner_geometry->getGeometryTypeId() == GEOS_POINT){
            const GWSPoint* point = dynamic_cast<const GWSPoint*>(geom);
            return controller->createPoint( ProjectionConversor::getInstance()->convert( point->getRepresentativeCoordinate(), from_epsg_code, to_epsg_code ) );

        // LINESTRING
        } else if( geom->inner_geometry->getGeometryTypeId() == GEOS_LINESTRING ){
            const GWSLineString* line = dynamic_cast<const GWSLineString*>(geom);
            QList<GWSCoordinate> coordinates;

            for(unsigned int i = 0; i < line->getCoordinates().size(); i++){
                GWSCoordinate coord = line->getCoordinates().at( i );
                coordinates.append( ProjectionConversor::getInstance()->convert( coord, from_epsg_code, to_epsg_code ) );
            }
            return controller->createLineString(coordinates);

        // POLYGON
         } else if( geom->inner_geometry->getGeometryTypeId() == GEOS_POLYGON ){
            const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>( geom->inner_geometry );

            QList< QList<GWSCoordinate> > rings;
            QList<GWSCoordinate> rings_coors;

            for(unsigned int i = 0; i < polygon->getExteriorRing()->getCoordinates()->size(); i++){
                geos::geom::Coordinate original_coord = 0;
                original_coord.x = polygon->getExteriorRing()->getCoordinates()->getAt( i ).x;
                original_coord.y = polygon->getExteriorRing()->getCoordinates()->getAt( i ).y;
                rings_coors.append( ProjectionConversor::getInstance()->convert( GWSCoordinate(original_coord.x, original_coord.y), from_epsg_code, to_epsg_code ) );
            }
            rings.append(rings_coors);

            for(unsigned int i = 0; i < polygon->getNumInteriorRing(); i++){
                QList<GWSCoordinate> rings_coors;

                for(unsigned int j = 0; j < polygon->getInteriorRingN( i )->getCoordinates()->size(); j++){
                    geos::geom::Coordinate original_coord = 0;
                    original_coord.x = polygon->getInteriorRingN( i )->getCoordinates()->getAt( j ).x;
                    original_coord.y = polygon->getInteriorRingN( i )->getCoordinates()->getAt( j ).y;
                    rings_coors.append( ProjectionConversor::getInstance()->convert( GWSCoordinate(original_coord.x, original_coord.y), from_epsg_code, to_epsg_code ) );
                }
                rings.append(rings_coors);
            }
            return controller->createPolygon( rings );

        } else {
            qWarning() << QString( "Unknown geometry type: %1").arg( QString::fromStdString( geom->inner_geometry->getGeometryType() ) );
        }*/

    } catch ( ... ){        
        qWarning() << QString("Geometry type parsing crashed" );
    }
}

GWSAreaUnit GWSGeometryUtils::getGeometryArea( const GWSGeometry* geom, const GWSGeometryFactory* controller ){
   return this->reprojectGeometry( geom, controller, 4326 , 23030 )->getArea();
}

GWSGeometry* GWSGeometryUtils::getIntersectingPolygonFromMultipolygon( const GWSGeometry* multipolygon, const GWSGeometry* geom, const GWSGeometryFactory* controller ){
     geos::geom::Geometry* intersecting_geom;

     for( unsigned int i = 0; i < multipolygon->inner_geometry->getNumGeometries(); i++ ){
         if ( multipolygon->inner_geometry->getGeometryN( i )->intersects( geom->inner_geometry ) ){
             intersecting_geom = multipolygon->inner_geometry->getGeometryN( i )->clone();
             break;
         }
     }
     return controller->createGeometry( QString::fromStdString( intersecting_geom->toString() ) );
}
