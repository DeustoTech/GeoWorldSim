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
    virtual bool continueToNext();

private slots:
    virtual bool behave();

protected:
    QList<GWSCoordinate> route_nodes;
};

#endif // LOOPOVERDESTINATIONSBEHAVIOUR_H
