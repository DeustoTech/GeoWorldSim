#ifndef GWSPOLYGON_H
#define GWSPOLYGON_H

#include "Geometry.h"
#include "LineString.h"
#include "../../geos/geom/Polygon.h"

class GWSPolygon : public GWSGeometry
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GWSPolygon( GWSLineString outer_ring = GWSLineString() , QList<GWSLineString> inner_rings = QList<GWSLineString>() );

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;

    // GETTERS
    double getMinX() const;
    double getMaxX() const;
    double getMinY() const;
    double getMaxY() const;

    // METHODS

private:
    GWSPolygon( geos::geom::Polygon* inner_geometry );
};

Q_DECLARE_METATYPE(GWSPolygon*)

#endif // GWSPOLYGON_H
