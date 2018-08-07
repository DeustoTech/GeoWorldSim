#ifndef PASTUREAGENT_H
#define PASTUREAGENT_H

#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"



class PastureAgent : public GWSAgent
{
    Q_OBJECT // Needed macro

public:
    // CONSTRUCTOR
    Q_INVOKABLE explicit PastureAgent( QObject* parent = Q_NULLPTR );

    // METHODS
    virtual void behave();
};

#endif // PASTUREAGENT_H
