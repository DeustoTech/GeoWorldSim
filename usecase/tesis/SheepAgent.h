#ifndef SHEEPAGENT_H
#define SHEEPAGENT_H

#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

#include "../../app/App.h"

#include "../../skill/view/ViewSkill.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../object/ObjectFactory.h"

class SheepAgent : public GWSAgent
{
    Q_OBJECT // Needed macro

public:

    // CONSTRUCTOR
    Q_INVOKABLE explicit SheepAgent( QObject* parent = Q_NULLPTR );
    ~SheepAgent();

    virtual void behaveOld();
};

Q_DECLARE_METATYPE(SheepAgent*) // REQUIRED IN EVERY CHILD

#endif // SHEEPAGENT_H
