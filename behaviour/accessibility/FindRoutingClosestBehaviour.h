#ifndef FINDROUTINGCLOSESTBEHAVIOUR_H
#define FINDROUTINGCLOSESTBEHAVIOUR_H

#include "../../util/geometry/Coordinate.h"
#include "../../behaviour/Behaviour.h"

class FindRoutingClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindRoutingClosestBehaviour();

    // PROPERTIES
    static QString ENTITY_TO_ACCESS_TYPE;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_ROUTING_CLOSEST_ID_AS;
    static QString STORE_ROUTING_CLOSEST_ROUTE_AS;
    static QString STORE_ROUTING_DISTANCE_AS;
    static QString STORE_ROUTING_CLOSEST_X_AS;
    static QString STORE_ROUTING_CLOSEST_Y_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_ROUTING_CLOSEST_FOUND;

protected:

    GWSCoordinate closest_coor_and_route;
    QPair< GWSCoordinate, QString > closest_coor_id;

protected slots:

     virtual QPair< double , QJsonArray > behave();

};

#endif // FINDROUTINGCLOSESTBEHAVIOUR_H
