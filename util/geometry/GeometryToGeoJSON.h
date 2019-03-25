#ifndef GWSGEOMETRYTOGEOJSON_H
#define GWSGEOMETRYTOGEOJSON_H

#include <QJsonObject>
#include "geos/geom/Geometry.h"

class GWSGeometryToGeoJSON
{
public:

    static geos::geom::Geometry* GeometryFromGeoJSON( QJsonObject geojson );
    static QJsonObject GeometryToGeoJSON( geos::geom::Geometry* geometry );

};

#endif // GWSGEOMETRYTOGEOJSON_H
