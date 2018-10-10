#ifndef LOOPOVERDESTINATIONSBEHAVIOUR_H
#define LOOPOVERDESTINATIONSBEHAVIOUR_H


#include "../../behaviour/Behaviour.h"

class MoveAlongTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveAlongTSPRouteBehaviour();

    // GETTERS
    QList<GWSCoordinate> generateRouteCoordinateArray() const;
    void generateOrderedTSPRoute();
    virtual bool continueToNext();

private slots:
    virtual bool behave();

protected:
    QList< QPair< GWSCoordinate , QString > > route;
};

#endif // LOOPOVERDESTINATIONSBEHAVIOUR_H
