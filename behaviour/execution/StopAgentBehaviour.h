#ifndef STOPAGENTBEHAVIOUR_H
#define STOPAGENTBEHAVIOUR_H

#include "../Behaviour.h"

class StopAgentBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StopAgentBehaviour();

    // METHODS
    virtual bool canContinueToNext(); // Behaviour finished check

private slots:
    virtual bool behave();
};

#endif // STOPAGENTBEHAVIOUR_H
