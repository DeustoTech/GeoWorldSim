#ifndef PREDATORAGENT_H
#define PREDATORAGENT_H

#include "../../agent/Agent.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

#include "../../app/App.h"

#include "../../skill/view/ViewSkill.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../object/ObjectFactory.h"

class PredatorAgent : public GWSAgent
{
    Q_OBJECT // NEEDED MACRO

public:

    // CONSTRUCTOR
    Q_INVOKABLE explicit PredatorAgent();
    ~PredatorAgent();

};

Q_DECLARE_METATYPE(PredatorAgent*) // REQUIRED IN EVERY CHILD

#endif // PREDATORAGENT_H
