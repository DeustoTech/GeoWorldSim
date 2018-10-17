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
    static QString CLOSEST_AGENT_SUBTYPE;
    static QString CLOSEST_ID;
    static QString NEXTS;


protected slots:
    virtual QStringList behave();
    GWSDijkstraRouting* routing_graph = Q_NULLPTR;
    GWSCoordinate closest_coor;
    QPair< GWSCoordinate, QString > closest_coor_id;
};

#endif // FINDCLOSESTBEHAVIOUR_H
