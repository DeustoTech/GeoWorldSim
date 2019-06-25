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
    static QString MOVING_TOWARDS;
    static QString ACCUMULATED_DISTANCE;
    static QString ACCUMULATED_TIME;

    // GETTERS
    //GWSSpeedUnit getCurrentSpeed();
    //GWSCoordinate getMovingTowardsCoordinate();

    // SETTERS
    //void setMaxSpeed( GWSSpeedUnit max_speed ); // In m/s

    // METHODS
    virtual GWSSpeedUnit calculateNewSpeed( const GWSSpeedUnit &current_speed , const GWSSpeedUnit &max_speed , double force = 0 ); // force[0,1) Accelerate / force(-1,0] Brake
    virtual void move( const GWSTimeUnit &movement_duration , const GWSSpeedUnit &movement_speed , const GWSGeometry &movement_towards_geom );

private:


};

#endif // MOVESKILL_H
