#ifndef NOMINATIMAPIDRIVER_H
#define NOMINATIMAPIDRIVER_H

#include <QObject>

#include "geos/geom/Coordinate.h"
#include "utils/api/SyncAPIDriver.h"

class NominatimAPIDriver : public QObject
{
    Q_OBJECT
public:
    NominatimAPIDriver(QObject* parent = 0);
    ~NominatimAPIDriver();

    // METHODS
    void getLocation( QString place_name );
    void getPlaceInfo( geos::geom::Coordinate coor , int zoom = 18);

};

#endif // NOMINATIMAPIDRIVER_H
