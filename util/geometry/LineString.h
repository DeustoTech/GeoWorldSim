#ifndef GSSLINESTRING_H
#define GSSLINESTRING_H

#include <QList>

#include "../../util/geometry/Geometry.h"
#include "../../geos/geom/LineString.h"

class GWSLineString : public GWSGeometry
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
    GWSLengthUnit getLength() const;

    // METHODS (Caller takes ownership of returned geometry)
    GWSLineString* createReversed() const;
    QList<GWSLineString*> createChopsFromCoordinates() const;
    QList<GWSLineString*> createAmountChops( int amount ) const;
    virtual GWSLineString* createCopy() const;

protected:
    explicit GWSLineString( geos::geom::LineString* geom , GWSObject *parent = 0);

};

#endif // GSSLINESTRING_H
