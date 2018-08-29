#ifndef GWSGEOMETRY_H
#define GWSGEOMETRY_H

#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequenceFilter.h"

#include <QJsonObject>
#include <QImage>
#include <QColor>

#include "../../object/Object.h"
#include "../units/Units.h"
#include "Coordinate.h"

using namespace geos::geom;

class GWSGeometry : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSGeometry();
    ~GWSGeometry();

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

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
    bool isInBounds( double minX , double maxX , double minY , double maxY ) const;
    bool intersects( const QSharedPointer<GWSGeometry> other) const;
    bool intersects( double minX , double maxX , double minY , double maxY ) const;
    bool equals( QSharedPointer<GWSGeometry> other ) const;
    GWSLengthUnit getDistance( QSharedPointer<GWSGeometry> other ) const;

    // SPATIAL TRANSFORMATIONS
    void transformMove( const GWSCoordinate &apply_movement );
    void transformBuffer( double threshold );
    void transformUnion( QSharedPointer<GWSGeometry> other );
    void transformIntersection( QSharedPointer<GWSGeometry> other );

protected:
    GWSGeometry(geos::geom::Geometry* inner_geometry);

    // INNER GEOMETRY
    geos::geom::Geometry* inner_geometry = Q_NULLPTR;
};

// SPATIAL TRANSFORM FILTERS
class TransformMoveFilter : public geos::geom::CoordinateSequenceFilter{
public:
    TransformMoveFilter(const GWSCoordinate &apply_movement );
    virtual void filter_rw(CoordinateSequence&  seq , std::size_t i );
    virtual void filter_ro(CoordinateSequence&  seq , std::size_t i );
    virtual bool isDone() const;
    virtual bool isGeometryChanged() const;
    GWSCoordinate apply_movement;
    QVector<int> moved_coor_indexes;
    bool finished = false;
};

Q_DECLARE_METATYPE( QSharedPointer<GWSGeometry> )

#endif // GWSGEOMETRY_H
