#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include <QString>
#include <QJsonObject>
#include "../../util/geometry/Coordinate.h"
#include "geos/geom/Geometry.h"

#include "../../util/units/Units.h"

using namespace geos::geom;

struct GWSGeometry {

    // PROPERTIES
    QJsonObject geojson;
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;

    // CONSTRUCTORS
    GWSGeometry(){}
    GWSGeometry( QJsonObject geojson );
    GWSGeometry(const GWSGeometry &other) : GWSGeometry(other.geojson){}
    ~GWSGeometry();

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    GWSCoordinate getCentroid() const;

    // OPERATORS
    bool operator == (const GWSGeometry&) const;
    bool operator != (const GWSGeometry&) const;
    bool operator < (const GWSGeometry&) const;
    GWSGeometry& operator= (const GWSGeometry&);

};

#endif // GWSGEOMETRY_H
