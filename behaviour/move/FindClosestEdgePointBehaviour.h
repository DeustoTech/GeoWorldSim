#ifndef GETTONEARESTGRAPHEDGEBEHAVIOUR_H
#define GETTONEARESTGRAPHEDGEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class FindClosestEdgePointBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindClosestEdgePointBehaviour();

    virtual bool canContinueToNext();

private slots:
    virtual bool behave();

};

#endif // GETTONEARESTGRAPHEDGEBEHAVIOUR_H
