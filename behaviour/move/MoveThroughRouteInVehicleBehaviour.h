#ifndef MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H
#define MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../skill/move/MoveThroughRouteInVehicleSkill.h"

class MoveThroughRouteInVehicleBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteInVehicleBehaviour();
    ~MoveThroughRouteInVehicleBehaviour();


protected slots:
    virtual QJsonArray behave();
};

#endif // MOVETHOUGHROUTEINVEHICLEBEHAVIOUR_H
