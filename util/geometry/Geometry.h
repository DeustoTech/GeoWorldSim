#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include "../../object/Object.h"
#include "geos/geom/Geometry.h"

#include <QJsonObject>
#include <QPointF>
#include <QImage>
#include <QColor>

#include "../units/Units.h"
#include "Coordinate.h"

using namespace geos::geom;

class GWSGeometry : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSGeometry();
    GWSGeometry( GWSCoordinate coor ); // Point
    ~GWSGeometry();

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;
    virtual QString toString() const;

    // GETTERS
    bool isValid() const;
    GWSAreaUnit getArea() const;
    double getMaxX() const;
    double getMinX() const;
    double getMaxY() const;
    double getMinY() const;
    GWSCoordinate getCentroid() const;

    // SPATIAL COMPARATORS
    bool intersects( const GWSGeometry* other) const;
    bool equals( const GWSGeometry* other ) const;
    GWSLengthUnit getDistance( const GWSGeometry* other ) const;

    // SPATIAL TRANSFORMATIONS
    void transformBuffer( double threshold );
    void transformUnion( const GWSGeometry* other );
    void transformIntersection( const GWSGeometry* other );

protected:
    GWSGeometry(const geos::geom::Geometry* inner_geometry);

    // INNER GEOMETRY
    const geos::geom::Geometry* inner_geometry = Q_NULLPTR;
};

Q_DECLARE_METATYPE(GWSGeometry*)

#endif // GWSGEOMETRY_H
