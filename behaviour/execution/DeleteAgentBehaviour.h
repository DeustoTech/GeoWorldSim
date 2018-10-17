#ifndef DELETEAGENTBEHAVIOUR_H
#define DELETEAGENTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DeleteAgentBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit DeleteBehaviour();

    virtual bool canContinueToNext(); // Behaviour finished check

private slots:
    virtual bool behave();

private:
    bool deleted = false;
};

#endif // DELETEAGENTBEHAVIOUR_H
