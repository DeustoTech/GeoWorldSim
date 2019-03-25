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
    GWSCoordinate getCentroid() const;

    // OPERATORS
    bool operator == (const GWSNewGeometry&) const;
    bool operator != (const GWSNewGeometry&) const;
    bool operator < (const GWSNewGeometry&) const;
    GWSNewGeometry& operator= (const GWSNewGeometry&);

};

#endif // GWSNEWGEOMETRY_H
