#ifndef GSSGEOMETRY_H
#define GSSGEOMETRY_H

#include <QObject>
#include "geos/geom/Geometry.h"

#include <QJsonObject>
#include <QImage>
#include <QColor>

#include "util/units/Units.h"
#include "util/geometry/Coordinate.h"
#include "util/geometry/Envelope.h"

QT_FORWARD_DECLARE_CLASS(GWSPoint)
QT_FORWARD_DECLARE_CLASS(GWSGeometryFactory)

using namespace geos::geom;

class GWSGeometry : public QObject
{
    Q_OBJECT
    friend class GWSGeometryFactory; // GWSGeometryFactory can access this constructor
    friend class GWSGeometryUtils;

public:
    ~GWSGeometry();

    // EXPORTERS
    virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;
    virtual QJsonObject toGeoJSON( QJsonObject properties = QJsonObject() ) const;
    virtual QString toString() const;

    // GETTERS
    bool isValid() const;
    bool intersects( const GWSGeometry *other) const;
    bool equals( const GWSGeometry* other ) const;
    GWSCoordinate getRepresentativeCoordinate() const;
    QList<GWSCoordinate> getCoordinates() const;
    virtual GWSAreaUnit getArea() const;
    virtual GWSCoordinate getCentroid() const;
    virtual GWSEnvelope getEnvelope() const;
    virtual double getDistance( const GWSGeometry* other ) const;
    virtual GWSCoordinate getFirstCoordinate() const;
    virtual GWSCoordinate getLastCoordinate() const;

    // METHODS (Caller takes ownership of returned geometry)
    virtual GWSGeometry* createCopy() const;
    GWSPoint* createCentroid() const;
    GWSGeometry* createBuffer( double buffer ) const;
    GWSGeometry* createUnion( GWSGeometry* other ) const;
    GWSGeometry* createIntersection( const GWSGeometry* other ) const;

protected:
    explicit GWSGeometry( geos::geom::Geometry* geom , QObject *parent = Q_NULLPTR );

    const geos::geom::Geometry* inner_geometry = 0;

private:
    GWSGeometry* castToGeometryType( geos::geom::Geometry* geom ) const;

};

#endif // GSSGEOMETRY_H
