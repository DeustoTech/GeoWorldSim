#ifndef GWSPOINT_H
#define GWSPOINT_H

#include "Geometry.h"
#include "geos/geom/Point.h"

class GWSPoint : public GWSGeometry
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSPoint( double x = NAN , double y = NAN , double z = NAN );

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    double getX() const;
    double getY() const;
    double getZ() const;

private:
    GWSPoint( geos::geom::Point* inner_geometry );
};

Q_DECLARE_METATYPE(GWSPoint*)

#endif // GWSPOINT_H
