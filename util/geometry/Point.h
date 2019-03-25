#ifndef GWSPOINT_H
#define GWSPOINT_H

#include "OldGeometry.h"
#include "geos/geom/Point.h"

class GWSPoint : public GWSOldGeometry
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSPoint( double x = 0 , double y = 0 , double z = 0 );

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
