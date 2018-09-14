#ifndef MOVETSPBEHAVIOUR_H
#define MOVETSPBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveTSPBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveTSPBehaviour();
    ~MoveTSPBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();
};

#endif // MOVETSPBEHAVIOUR_H
