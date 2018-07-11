#ifndef GSSGEOMETRYUTILS_H
#define GSSGEOMETRYUTILS_H

#include <QObject>
#include "util/geometry/Geometry.h"
#include "util/geometry/GeometryFactory.h"

class GWSGeometryUtils : public QObject
{
    Q_OBJECT

public:
    static GWSGeometryUtils* globalInstance();

    GWSGeometry* simplifyDouglasPeucker(GWSGeometry* geom, int distance_tolerance);
    GWSGeometry* reprojectGeometry(const GWSGeometry* geom, const GWSGeometryFactory* controller, int from_epsg_code , int to_epsg_code);
    GWSAreaUnit getGeometryArea( const GWSGeometry* geom, const GWSGeometryFactory* controller );
    GWSGeometry* getIntersectingPolygonFromMultipolygon( const GWSGeometry* multipolygon, const GWSGeometry* geom, const GWSGeometryFactory* controller );

private:
    explicit GWSGeometryUtils(QObject *parent = 0);
    GWSGeometryUtils(GWSGeometryUtils const&);
    ~GWSGeometryUtils();

};

#endif // GSSGEOMETRYUTILS_H
