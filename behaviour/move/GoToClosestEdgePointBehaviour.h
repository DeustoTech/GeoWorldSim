#ifndef GOTOHALLBEHAVIOUR_H
#define GOTOHALLBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GoToClosestEdgePointBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GoToClosestEdgePointBehaviour();
    virtual bool canContinueToNext();

protected slots:
    bool behave();
};

#endif // GOTOHALLBEHAVIOUR_H
