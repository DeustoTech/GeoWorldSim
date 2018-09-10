#ifndef MOVETHROUGHROUTEBEHAVIOUR_H
#define MOVETHROUGHROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveThroughRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveThroughRouteBehaviour();
    ~MoveThroughRouteBehaviour();

    // GETTERS
    virtual bool finished();

private slots:
    virtual bool behave();

};

#endif // MOVETHROUGHROUTEBEHAVIOUR_H

