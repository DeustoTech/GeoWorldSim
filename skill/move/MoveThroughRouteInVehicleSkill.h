#ifndef MOVETHROUGHROUTEINVEHICLESKILL_H
#define MOVETHROUGHROUTEINVEHICLESKILL_H

#include "../../skill/move/MoveThroughRouteSkill.h"

class MoveThroughRouteInVehicleSkill : public MoveThroughRouteSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteInVehicleSkill();

    // METHODS
    virtual void moveAndPollute(  GWSTimeUnit movement_duration  );
};



#endif // MOVETHROUGHROUTEINVEHICLESKILL_H
