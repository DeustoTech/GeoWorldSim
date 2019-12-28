#include "GeoJsonToGeometryConversor.h"

#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateSequenceFactory.h"

GWSGeometry* GeoJsonToGeometryConversor::format( const GWSGeometryFactory* geom_controller , QJsonObject geojson){
    if ( geojson.isEmpty() ) {
        return 0;
    }

    QJsonObject geometry = geojson;
    if( geojson.contains("geometry") ){
        geometry = geojson["geometry"].toObject();
    }

    // http://geojson.org/geojson-spec.html

    if( geometry["type"].toString().toLower() == "point" ){

        QJsonArray c = geometry["coordinates"].toArray();
        GWSCoordinate coor( c[0].toDouble() , c[1].toDouble() , (c.size() > 2) ? c[2].toDouble() : 0);
        return geom_controller->createPoint( coor );
    }

    if( geometry["type"].toString().toLower() == "linestring" ){

        QJsonArray l = geometry["coordinates"].toArray();
        QList<GWSCoordinate> coordinates;

        for(int i = 0; i < l.size(); i++){
            QJsonArray c = geometry["coordinates"].toArray().at(i).toArray();
            GWSCoordinate coor(c[0].toDouble() , c[1].toDouble() , (c.size() > 2) ? c[2].toDouble() : 0);
            coordinates.append(coor);
        }
        return geom_controller->createLineString( coordinates );
    }

    if( geometry["type"].toString().toLower() == "polygon" ){

        QJsonArray p = geometry["coordinates"].toArray();
        QList< QList<GWSCoordinate> > rings_coors;

        for(int i = 0; i < p.size(); i++){
            QJsonArray l = geometry["coordinates"].toArray().at(i).toArray();
            QList<GWSCoordinate> coordinates;
            for(int j = 0; j < l.size(); j++){
                QJsonArray c = geometry["coordinates"].toArray().at(i).toArray().at(j).toArray();
                GWSCoordinate coor(c[0].toDouble() , c[1].toDouble() , (c.size() > 2) ? c[2].toDouble() : 0);
                coordinates.append(coor);
            }
            rings_coors.append( coordinates );
        }
        return geom_controller->createPolygon( rings_coors );
    }
    return 0;
}

