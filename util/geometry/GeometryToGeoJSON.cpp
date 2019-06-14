#include "GeometryToGeoJSON.h"

#include <QDebug>
#include <QJsonArray>

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/CoordinateSequenceFilter.h"

#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"

geos::geom::Geometry* GWSGeometryToGeoJSON::GeometryFromGeoJSON( const QJsonObject &geojson ){

    QString geom_type = geojson.value("type").toString();
    if( geom_type.isEmpty() ){ return Q_NULLPTR; }

    const geos::geom::GeometryFactory* factory = geos::geom::GeometryFactory::getDefaultInstance();
    geos::geom::Geometry* geom = Q_NULLPTR;
    QJsonArray coors = geojson.value("coordinates").toArray();

    try {

        if( geom_type.toLower() == "point" ){
            geom = factory->createPoint(
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
            geom = factory->createLineString( seq );
        } else if ( geom_type.toLower() == "polygon" ){

            geos::geom::LinearRing* outer_ring = Q_NULLPTR;
            std::vector<geos::geom::Geometry *>* holes = new std::vector<geos::geom::Geometry*>();

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

                if( i == 0 && seq->size() >= 4 ){
                    outer_ring = factory->createLinearRing( seq );
                } else if( seq->size() >= 4 ) {
                    holes->push_back( factory->createLinearRing( seq ) );
                }
            }

            if( outer_ring ){
                geom = factory->createPolygon( outer_ring , holes );
            }

        } else {
            qWarning() << "Unknown geometry type" << geom_type;
        }

    } catch ( std::exception &e ){
        qWarning() << "Error creating geometry" << e.what();
    }

    return geom;

}

QJsonObject GWSGeometryToGeoJSON::GeometryToGeoJSON(geos::geom::Geometry *geometry){

    QJsonObject json_geometry;
    if( !geometry ){ return json_geometry; }

    try {

        // POINT
        if( geometry->getGeometryTypeId() == geos::geom::GEOS_POINT ){
                json_geometry.insert("type", QString("Point") );

                QJsonArray coordinates;
                const geos::geom::Coordinate* coor = geometry->getCoordinate();
                if( coor ){
                    coordinates.append( coor->x );
                    coordinates.append( coor->y );
                    coordinates.append( coor->z );
                }
                json_geometry.insert("coordinates", coordinates);
                return json_geometry;
        }

        // LINESTRING
        if( geometry->getGeometryTypeId() == geos::geom::GEOS_LINESTRING ){

                geos::geom::LineString* line = dynamic_cast<geos::geom::LineString*>( geometry->clone() );
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
        if( geometry->getGeometryTypeId() == geos::geom::GEOS_POLYGON ) {
                geos::geom::Polygon* polygon = dynamic_cast<geos::geom::Polygon*>( geometry->clone() );

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
        qWarning() << QString("Geometry type %1 parsing crashed").arg( QString::fromStdString( geometry->getGeometryType() ) );
    }

    return json_geometry;
}
