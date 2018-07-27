#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include "geos/geom/Geometry.h"

#include <QJsonObject>
#include <QImage>
#include <QColor>

#include "../units/Units.h"
#include "Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSAgent)

using namespace geos::geom;

class GWSGeometry
{

public:
    GWSGeometry( GWSAgent* agent );
    ~GWSGeometry();

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;
    virtual QString toString() const;

    // GETTERS
    bool isGeometryValid() const;
    GWSAreaUnit getArea() const;
    double getGeometryMaxX() const;
    double getGeometryMinX() const;
    double getGeometryMaxY() const;
    double getGeometryMinY() const;
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
    GWSGeometry(geos::geom::Geometry* inner_geometry);

    // REF AGENT
    GWSAgent* agent = Q_NULLPTR;

    // INNER GEOMETRY
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;
};

#endif // GWSGEOMETRY_H
