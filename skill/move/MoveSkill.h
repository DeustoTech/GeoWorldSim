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
    static QString MAX_SPEED;
    static QString CURRENT_SPEED;
    static QString MOVING_TOWARDS_X;
    static QString MOVING_TOWARDS_Y;
    static QString ACCUMULATED_DISTANCE;
    static QString ACCUMULATED_TIME;

    // GETTERS
    //GWSSpeedUnit getCurrentSpeed();
    //GWSCoordinate getMovingTowardsCoordinate();

    // SETTERS
    //void setMaxSpeed( GWSSpeedUnit max_speed ); // In m/s

    // METHODS
    virtual GWSSpeedUnit calculateNewSpeed( GWSSpeedUnit current_speed , GWSSpeedUnit max_speed , double force = 0 ); // force[0,1) Accelerate / force(-1,0] Brake
    virtual void move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate movement_towards );

signals:
    void speedChangedSignal( GWSSpeedUnit current_speed );
    void movedSignal();

private:


};

#endif // MOVESKILL_H
