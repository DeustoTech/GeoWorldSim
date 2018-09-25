#ifndef LOOPOVERDESTINATIONSBEHAVIOUR_H
#define LOOPOVERDESTINATIONSBEHAVIOUR_H


#include "../../behaviour/Behaviour.h"

class LoopOverRouteStagesBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit LoopOverRouteStagesBehaviour();

    // GETTERS
    QList<GWSCoordinate> generateRouteCoordinateArray() const;
    QList < QPair < GWSCoordinate , QString > > generateOrderedTSPRoute() ;
    virtual bool continueToNext();

private slots:
    virtual bool behave();

protected:
    QList<GWSCoordinate> route_nodes;
    QList < QPair < GWSCoordinate , QString > > ordered_container_tsp_route_coord_id_array;
};

#endif // LOOPOVERDESTINATIONSBEHAVIOUR_H
