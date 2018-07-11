#ifndef GWSBEHAVIOUR_H
#define GWSBEHAVIOUR_H

#include "object/Object.h"
#include "agent/Agent.h"

class GWSBehaviour : public GWSObject
{
    Q_OBJECT
    friend class GWSAgent; // Agent can check behaviour insides

public:
    explicit GWSBehaviour( GWSAgent* behaving_agent );

    // GETTERS
    GWSAgent* getAgent();
    GWSBehaviour* getNext();
    virtual bool finished(); // Behaviour finished check

protected:

    QList<GWSBehaviour*> sub_behaviours;
    GWSBehaviour* next_behaviour = Q_NULLPTR;

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    void tick(); // Acts as a behave() wrapper
    virtual bool behave(); // Behaviour, To be implemented by children, must be synchronous because tick() is already asyncrhonous

};

#endif // GWSBEHAVIOUR_H
