#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include <QString>
#include <QJsonObject>
#include "../../util/geometry/Coordinate.h"
#include "geos/geom/Geometry.h"

#include "../../util/units/Units.h"

using namespace geos::geom;

struct GWSGeometry {

    friend class GWSGeometryGetters;
    friend class GWSGeometryComparators;
    friend class GWSGeometryTransformators;

public:

    // CONSTRUCTORS
    GWSGeometry(){}
    GWSGeometry( const QJsonObject &geojson );
    GWSGeometry( const GWSCoordinate &coor );
    GWSGeometry( const GWSGeometry &other );
    ~GWSGeometry();

    // GETTERS
    QString getUID() const;
    QJsonObject getGeoJSON() const;
    bool isValid() const;
    GWSCoordinate getCentroid() const;
    GWSLengthUnit getDistance( const GWSGeometry& other ) const;

    // OPERATORS
    bool operator == (const GWSGeometry&) const;
    bool operator != (const GWSGeometry&) const;
    bool operator < (const GWSGeometry&) const;
    GWSGeometry& operator= (const GWSGeometry&);

private:

    // PROPERTIES
    QJsonObject geojson;
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;

};

#endif // GWSGEOMETRY_H
