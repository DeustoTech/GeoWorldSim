#ifndef ENCODEDPOLYLINECONVERSOR_H
#define ENCODEDPOLYLINECONVERSOR_H

#include <QObject>

#include "utils/geometry/Coordinate.h"

using namespace geos::geom;

class EncodedPolylineConversor: public QObject
{
    Q_OBJECT
public:
    explicit EncodedPolylineConversor(QObject *parent = 0);

    // METHODS
    QList<GWSCoordinate> decodePolyline( QString encoded);

private:
    double PRECISION = 1E5;

};

#endif // ENCODEDPOLYLINECONVERSOR_H
