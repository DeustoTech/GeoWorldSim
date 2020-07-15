#ifndef MOVETOPOINTBEHAVIOUR_H
#define MOVETOPOINTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"


namespace geoworldsim {
namespace behaviour {

class MoveToPointBehaviour : public Behaviour
{   
 Q_OBJECT

public:
    Q_INVOKABLE explicit MoveToPointBehaviour();
    ~MoveToPointBehaviour();

    // PROPERTIES
    static QString INPUT_ROUTE_DESTINATION;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;
    static QString INPUT_TRANSPORT_NETWORK_TYPE;

    static QString CURRENT_ROAD_ID;
    static QString CURRENT_ROAD_TYPE;
    static QString CURRENT_ROAD_MAXSPEED;
    static QString STORE_ROUTE_AS;

    static QString INSTANT_DISTANCE;
    static QString ACCUMULATED_DISTANCE;
    static QString INSTANT_TIME;
    static QString ACCUMULATED_TIME;

protected slots:
    virtual QPair< double , QJsonArray > behave();

};

#endif // MOVETOPOINTBEHAVIOUR_H

}
}
