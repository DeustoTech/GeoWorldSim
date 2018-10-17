#ifndef FINDCLOSESTBEHAVIOUR_H
#define FINDCLOSESTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../util/routing/DijkstraRouting.h"


class FindClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindClosestBehaviour();

    virtual void generateGraph();

    // PROPERTIES
    static QString CLOSEST_AGENT_TYPE;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_CLOSEST_ID_AS;
    //static QString STORE_CLOSEST_ROUTE_AS;
    static QString STORE_CLOSEST_ROUTE_DISTANCE_AS;
    //static QString CLOSEST_FROM_X;
    //static QString CLOSEST_FROM_Y;
    static QString NEXTS;

protected:
    GWSDijkstraRouting* routing_graph = Q_NULLPTR;
    GWSCoordinate closest_coor;
    QPair< GWSCoordinate, QString > closest_coor_id;
protected slots:
    virtual QStringList behave();

};

#endif // FINDCLOSESTBEHAVIOUR_H
