#ifndef STOPAGENTBEHAVIOUR_H
#define STOPAGENTBEHAVIOUR_H

#include "../Behaviour.h"

class StopAgentBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StopAgentBehaviour();

private slots:
    virtual QStringList behave();
};

#endif // STOPAGENTBEHAVIOUR_H
