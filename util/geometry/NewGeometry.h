#ifndef GWSNEWGEOMETRY_H
#define GWSNEWGEOMETRY_H

#include <QString>
#include <QJsonObject>
#include "../../util/geometry/Coordinate.h"
#include "geos/geom/Geometry.h"

#include "../../util/units/Units.h"

using namespace geos::geom;

struct GWSNewGeometry {

    // PROPERTIES
    QJsonObject geojson;
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;

    // CONSTRUCTORS
    GWSNewGeometry(){}
    GWSNewGeometry( QJsonObject geojson );
    GWSNewGeometry(const GWSNewGeometry &other) : GWSNewGeometry(other.geojson){}
    ~GWSNewGeometry();

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    GWSAreaUnit getArea() const;
    double getGeometryMaxX() const;
    double getGeometryMinX() const;
    double getGeometryMaxY() const;
    double getGeometryMinY() const;
    GWSCoordinate getCentroid() const;
    QList<GWSCoordinate> getCoordinates() const;

    // OPERATORS
    bool operator == (const GWSNewGeometry&) const;
    bool operator != (const GWSNewGeometry&) const;
    bool operator < (const GWSNewGeometry&) const;
    GWSNewGeometry& operator= (const GWSNewGeometry&);

};

#endif // GWSNEWGEOMETRY_H
