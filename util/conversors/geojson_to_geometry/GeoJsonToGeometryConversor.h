#ifndef GEOJSONTOGEOMETRYCONVERSOR_H
#define GEOJSONTOGEOMETRYCONVERSOR_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include "util/geometry/GeometryFactory.h"
#include "geos/geom/Geometry.h"

using namespace geos::geom;

class GeoJsonToGeometryConversor : public QObject
{
    Q_OBJECT

public:
    static GWSGeometry* format( const GWSGeometryFactory* environment , QJsonObject geojson );
};

#endif // GEOJSONTOGEOMETRYCONVERSOR_H
