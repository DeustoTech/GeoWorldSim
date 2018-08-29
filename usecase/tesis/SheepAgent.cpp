#include "SheepAgent.h"


#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../skill/view/ViewSkill.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../object/ObjectFactory.h"

SheepAgent::SheepAgent() : GWSAgent() {
}

SheepAgent::~SheepAgent(){
}
