#include "ChangeDestinationBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../agent/Agent.h"

#include "../../behaviour/move/MoveBehaviour.h"

ChangeDestinationBehaviour::ChangeDestinationBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}

bool ChangeDestinationBehaviour::finished(){
    return false;
}


bool ChangeDestinationBehaviour::behave(){


}
