#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QString>
#include <QJsonObject>
#include "../../util/geometry/Coordinate.h"
#include "geos/geom/Geometry.h"

#include "../../util/units/Units.h"

namespace geoworldsim {
namespace geometry {


struct Geometry {

    friend class GeometryGetters;
    friend class GeometryComparators;
    friend class GeometryTransformators;

public:

    // CONSTRUCTORS
    Geometry(){}
    Geometry( const QJsonObject &geojson );
    Geometry( const Coordinate &coor );
    Geometry( const Geometry &other );
    ~Geometry();

    // GETTERS
    QString getUID() const;
    QJsonObject getGeoJSON() const;
    bool isValid() const;
    Coordinate getCentroid() const;
    unit::LengthUnit getDistance( const Geometry& other ) const;
    unit::LengthUnit getDistance( const Coordinate& other ) const;

    // OPERATORS
    bool operator == (const Geometry&) const;
    bool operator != (const Geometry&) const;
    bool operator < (const Geometry&) const;
    Geometry& operator= (const Geometry&);

private:

    // PROPERTIES
    QJsonObject geojson;
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;

};


}
}

#endif // GEOMETRY_H
