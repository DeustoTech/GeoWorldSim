#ifndef FINDCLOSESTBEHAVIOUR_H
#define FINDCLOSESTBEHAVIOUR_H

#include "../../util/geometry/Coordinate.h"
#include "../../behaviour/Behaviour.h"

class FindClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindClosestBehaviour();

    // PROPERTIES
    static QString CLOSEST_AGENT_TYPE;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_CLOSEST_ID_AS;
    //static QString STORE_CLOSEST_ROUTE_AS;
    static QString STORE_CLOSEST_ROUTE_DISTANCE_AS;
    static QString STORE_CLOSEST_X_AS;
    static QString STORE_CLOSEST_Y_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_CLOSEST_FOUND;

protected:

    GWSCoordinate closest_coor_and_route;
    QPair< GWSCoordinate, QString > closest_coor_id;

protected slots:

    virtual QJsonArray behave();

};

#endif // FINDCLOSESTBEHAVIOUR_H
