#ifndef GOTOHALLBEHAVIOUR_H
#define GOTOHALLBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GoToHallBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GoToHallBehaviour();
    virtual bool continueToNext();

protected slots:
    bool behave();
};

#endif // GOTOHALLBEHAVIOUR_H
