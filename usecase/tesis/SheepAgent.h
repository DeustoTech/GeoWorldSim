#ifndef SHEEPAGENT_H
#define SHEEPAGENT_H

#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

class SheepAgent : public GWSAgent
{
    Q_OBJECT // Needed macro

public:

    // CONSTRUCTOR
    Q_INVOKABLE explicit SheepAgent( QObject* parent = Q_NULLPTR );

    virtual void behave();
};

Q_DECLARE_METATYPE(SheepAgent*) // REQUIRED IN EVERY CHILD

#endif // SHEEPAGENT_H
