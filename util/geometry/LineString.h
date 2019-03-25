#ifndef GWSLINESTRING_H
#define GWSLINESTRING_H

#include <QList>

#include "OldGeometry.h"
#include "Point.h"
#include "geos/geom/LineString.h"

class GWSLineString : public GWSOldGeometry
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSLineString( QList<GWSPoint> points = QList<GWSPoint>() );

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024, QColor border_color = QColor("Black"), int border_width = 10 , QColor fill_color = QColor("Green"), QImage icon = QImage()) const;

    // GETTERS
    GWSLengthUnit getLength() const;

    // METHODS
    GWSLineString getReversed() const;

private:
    GWSLineString( geos::geom::LineString* inner_geometry );
};

Q_DECLARE_METATYPE(GWSLineString*)

#endif // GWSLINESTRING_H
