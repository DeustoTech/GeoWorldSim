#ifndef CONTAINERAGENT_H
#define CONTAINERAGENT_H


#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

class ContainerAgent : public GWSAgent
{
    Q_OBJECT // Needed macro

public:
    // CONSTRUCTOR
    Q_INVOKABLE explicit ContainerAgent();

    // METHODS
    virtual void behave();
};

Q_DECLARE_METATYPE(ContainerAgent*) // REQUIRED IN EVERY CHILD

#endif // CONTAINERAGENT_H
