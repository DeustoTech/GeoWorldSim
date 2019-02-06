#ifndef MOVESKILL_H
#define MOVESKILL_H

#include "../Skill.h"
#include "../../util/units/Units.h"
#include "../../util/geometry/Geometry.h"

class MoveSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveSkill();
    ~MoveSkill();

    // PROPERTIES
    static QString AGENT_MAX_SPEED_PROP;
    static QString AGENT_CURRENT_SPEED_PROP;
    static QString AGENT_MOVING_TOWARDS_X_PROP;
    static QString AGENT_MOVING_TOWARDS_Y_PROP;
    static QString AGENT_ACCUMULATED_DISTANCE_PROP;
    static QString AGENT_ACCUMULATED_TIME_PROP;

    // GETTERS
    GWSSpeedUnit getCurrentSpeed();
    GWSCoordinate getMovingTowardsCoordinate();

    // SETTERS
    void setMaxSpeed( GWSSpeedUnit max_speed ); // In m/s

    // METHODS
    virtual GWSSpeedUnit changeSpeed( double force = 0 ); // force[0,1) Accelerate / force(-1,0] Brake
    virtual void move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate movement_towards );

signals:
    void speedChangedSignal( GWSSpeedUnit current_speed );
    void movedSignal();

private:


};

#endif // MOVESKILL_H
