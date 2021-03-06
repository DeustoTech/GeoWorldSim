#ifndef FINDROUTINGCLOSESTBEHAVIOUR_H
#define FINDROUTINGCLOSESTBEHAVIOUR_H

#include "../../util/geometry/Coordinate.h"
#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class FindRoutingClosestBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindRoutingClosestBehaviour();

    // PROPERTIES
    static QString ENTITY_TYPE_TO_FIND;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_ROUTING_CLOSEST_ID_AS;
    static QString STORE_ROUTING_CLOSEST_ROUTE_AS;
    static QString STORE_ROUTING_DISTANCE_AS;
    static QString STORE_ROUTING_CLOSEST_GEOM_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_ROUTING_CLOSEST_FOUND;

protected slots:

     virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // FINDROUTINGCLOSESTBEHAVIOUR_H
