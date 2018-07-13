#ifndef GWSGEOMETRYCONTROLLER_H
#define GWSGEOMETRYCONTROLLER_H

#include <QObject>

#include "geos/geom/GeometryFactory.h"

#include "../../util/geometry/GeoCoordinates.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Point.h"
#include "../../util/geometry/LineString.h"
#include "../../util/geometry/Polygon.h"
#include "../../util/grid/Grid.h"

class GWSGeometryFactory : public GWSObject
{
    Q_OBJECT

public:
    static GWSGeometryFactory* globalInstance();

    // GETTERS
    //const geos::geom::GeometryFactory* getGeometryFactory() const;
    const GWSGrid getElevationModel() const;
    GeoCoordinates getRandomPoint( const GWSGeometry* bounds , unsigned int seed = qrand() ) const;

    // CREATORS
    GWSGeometry* createGeometry(QString wkt , bool elevate = true ) const;
    QString toWKT( GWSGeometry* geom ) const;

    GWSPoint* createPoint( GeoCoordinates coor , bool elevate = true ) const;
    GWSPoint* createPoint( double x , double y , double z = 0 , bool elevate = true ) const;

    GWSLineString* createLineString( GeoCoordinates c1 , GeoCoordinates c2 , bool elevate = true ) const;
    GWSLineString* createLineString( QList<GeoCoordinates> coors , bool elevate = true ) const;

    GWSPolygon* createPolygon( QList< QList<GeoCoordinates> > rings_coors , bool elevate = true ) const;
    GWSPolygon* createPolygon( QList<GeoCoordinates> outer_coors , bool elevate = true ) const;

    // ELEVATION SETTERS
    double getElevation( GeoCoordinates coor ) const;
    void setElevationModel( const GWSGrid& elevation );
    void setElevationPoint( GeoCoordinates coor , double value );

private:
    GWSGeometryFactory();
    GWSGeometryFactory(GWSGeometryFactory const&);
    ~GWSGeometryFactory();

    // SUBCREATORS
    GWSPoint* createPoint( geos::geom::Point* gpoint , bool elevate = true ) const;
    GWSLineString* createLineString( geos::geom::LineString* gline , bool elevate = true ) const;
    GWSPolygon* createPolygon( geos::geom::Polygon* gpolygon , bool elevate = true ) const;

    GWSGrid elevation_model; // Elevation model IN METERS
    const geos::geom::GeometryFactory* geometry_factory; // Geometry factory
};

#endif // GWSGEOMETRYCONTROLLER_H
