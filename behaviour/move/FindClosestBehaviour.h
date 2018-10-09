#ifndef FINDCLOSESTBEHAVIOUR_H
#define FINDCLOSESTBEHAVIOUR_H


#include "../Behaviour.h"
#include "../../skill/move/MoveSkill.h"
#include "../../util/routing/DijkstraRouting.h"


class FindClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit FindClosestBehaviour();
    ~FindClosestBehaviour();

    // GETTERS
    virtual bool continueToNext();

    // METHODS
    virtual void generateGraph();

protected:
    virtual bool behave();
    GWSDijkstraRouting* routing_graph = Q_NULLPTR;
    GWSCoordinate closest_coor;
    QPair< GWSCoordinate, QString > closest_coor_id;
};

#endif // FINDCLOSESTBEHAVIOUR_H
