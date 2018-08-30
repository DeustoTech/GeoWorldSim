#ifndef HUMANAGENT_H
#define HUMANAGENT_H

#include "../../agent/Agent.h"

class HumanAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE HumanAgent();
    // METHODS
    virtual void behave();
    ~HumanAgent();

};

Q_DECLARE_METATYPE(HumanAgent*) // REQUIRED IN EVERY CHILD

#endif // HUMANAGENT_H
