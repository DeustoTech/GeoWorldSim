#ifndef MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H
#define MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H

#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../skill/move/MoveThroughRouteInVehicleSkill.h"

class MoveThroughRouteInVehicleBehaviour : public MoveThroughRouteBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteInVehicleBehaviour();
    ~MoveThroughRouteInVehicleBehaviour();

protected:
    virtual void initialize();
};

#endif // MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H
