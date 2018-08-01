#ifndef PREDATORAGENT_H
#define PREDATORAGENT_H

#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

class PredatorAgent : public GWSAgent
{
    Q_OBJECT // NEEDED MACRO

public:

    // CONSTRUCTOR
    Q_INVOKABLE explicit PredatorAgent( QObject* parent = Q_NULLPTR );

    virtual void behave();
};

#endif // PREDATORAGENT_H
