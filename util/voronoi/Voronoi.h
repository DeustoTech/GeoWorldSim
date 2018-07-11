#ifndef VORONOI_H
#define VORONOI_H

#include <QObject>

#include "utils/geometry/Coordinate.h"
#include "utils/geometry/Envelope.h"
#include "utils/geometry/GeometryFactory.h"

class Voronoi : public QObject
{
    Q_OBJECT

public:
    explicit Voronoi(QObject *parent = 0);


    // METHODS
    QList<GWSGeometry*> createVoronoi( QList<GWSCoordinate> coordinates , const GWSEnvelope envelope , const GWSGeometryFactory* geometry_controller );
};

#endif // VORONOI_H
