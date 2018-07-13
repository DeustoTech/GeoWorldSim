#ifndef GSSPOLYGON_H
#define GSSPOLYGON_H

#include "../../util/geometry/Geometry.h"
#include "../../geos/geom/Polygon.h"

class GWSPolygon : public GWSGeometry
{
    Q_OBJECT
    friend class GWSGeometryFactory; // GeometryController can access this constructor
    friend class GSSGeometryUtils;
    friend class GWSGeometry;

public:

    // EXPORTERS
    virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;
    virtual QJsonObject toGeoJSON(QJsonObject properties) const;

    // GETTERS

    // METHODS (Caller takes ownership of returned geometry)

protected:
    explicit GWSPolygon( geos::geom::Polygon* geom , GWSObject *parent = 0);
};

#endif // GSSPOLYGON_H
