#ifndef MOVEONPARTICULARWASTETSPROUTE_H
#define MOVEONPARTICULARWASTETSPROUTE_H

#include "../../behaviour/Behaviour.h"

#include "../../util/geometry/Coordinate.h"

class CalculateTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CalculateTSPRouteBehaviour();

    // PARAMETERS
    static QString NEXTS;
    static QString WASTE_TYPE;
    static QString TSP_AGENT_TYPE;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_TSP_ROUTE_AS_PROPERTY_NAME;
    static QString STORE_TSP_DISTANCE_AS;
    static QString STORE_CLOSEST_ID_AS;
    static QString TSP_ROUTE_STAGE;
    static QString STORE_TSP_ROUTE_STAGE_X_AS;
    static QString STORE_TSP_ROUTE_STAGE_Y_AS;

protected slots:
    virtual QJsonArray behave();
};

#endif // MOVEONPARTICULARWASTETSPROUTE_H
