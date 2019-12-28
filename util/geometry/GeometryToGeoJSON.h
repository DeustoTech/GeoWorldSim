#ifndef GEOMETRYTOGEOJSON_H
#define GEOMETRYTOGEOJSON_H

#include <QJsonObject>
#include "geos/geom/Geometry.h"

namespace geoworldsim {
namespace geometry {


class GeometryToGeoJSON
{

    friend class Geometry;

public:
    static QJsonObject toGeoJSON( geos::geom::Geometry* geometry );

private:
    static geos::geom::Geometry* fromGeoJSON( const QJsonObject &geojson );

};

}
}

#endif // GEOMETRYTOGEOJSON_H
