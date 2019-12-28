#ifndef MOVETHROUGHROUTEINVEHICLESKILL_H
#define MOVETHROUGHROUTEINVEHICLESKILL_H

#include "../../skill/move/MoveThroughRouteSkill.h"

class MoveThroughRouteInVehicleSkill : public MoveThroughRouteSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteInVehicleSkill();

    // METHODS
    virtual void move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate route_destination , QString graph_type );
};



#endif // MOVETHROUGHROUTEINVEHICLESKILL_H
