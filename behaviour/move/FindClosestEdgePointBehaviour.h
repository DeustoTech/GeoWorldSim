#ifndef GETTONEARESTGRAPHEDGEBEHAVIOUR_H
#define GETTONEARESTGRAPHEDGEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GetToNearestGraphEdgeBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GetToNearestGraphEdgeBehaviour();

    virtual bool continueToNext();

private slots:
    virtual bool behave();

};

#endif // GETTONEARESTGRAPHEDGEBEHAVIOUR_H
