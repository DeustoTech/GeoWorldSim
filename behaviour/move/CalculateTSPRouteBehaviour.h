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
    static QString STORE_TSP_ROUTE_AS;
    static QString STORE_TSP_DISTANCE_AS;

    QList< QPair< GWSCoordinate , QString > > route;
    QList< QPair< GWSCoordinate , QString > > ordered_agents_to_visit_tsp_route_coord_id_array;
    QStringList ordered_agents_to_visit_tsp_route_id_array;

protected slots:
    virtual QStringList behave();

};

#endif // MOVEONPARTICULARWASTETSPROUTE_H
