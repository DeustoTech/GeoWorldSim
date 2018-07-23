#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include "../../object/Object.h"
#include "geos/geom/Geometry.h"

#include <QJsonObject>
#include <QImage>
#include <QColor>

#include "../../util/units/Units.h"

QT_FORWARD_DECLARE_CLASS(GWSPoint)
QT_FORWARD_DECLARE_CLASS(GWSGeometryFactory)

using namespace geos::geom;

class GWSGeometry : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSGeometry();
    ~GWSGeometry();

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;
    virtual QString toString() const;

    // GETTERS
    bool isValid() const;
    GWSPoint getRepresentativePoint() const;
    GWSAreaUnit getArea() const;

    // SPATIAL COMPARATORS
    bool intersects( const GWSGeometry other) const;
    bool equals( const GWSGeometry other ) const;
    GWSLengthUnit getDistance( const GWSGeometry other ) const;

    // SPATIAL TRANSFORMATIONS
    GWSPoint getCentroid() const;
    GWSGeometry getBuffer( double threshold ) const;
    GWSGeometry getUnion( const GWSGeometry other ) const;
    GWSGeometry getIntersection( const GWSGeometry other ) const;

protected:
    GWSGeometry(const geos::geom::Geometry* inner_geometry);

    // INNER GEOMETRY
    const geos::geom::Geometry* inner_geometry = Q_NULLPTR;
};

Q_DECLARE_METATYPE(GWSGeometry*)

#endif // GWSGEOMETRY_H
