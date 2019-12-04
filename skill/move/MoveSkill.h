#ifndef MOVESKILL_H
#define MOVESKILL_H

#include "../Skill.h"
#include "../../util/geometry/Geometry.h"

namespace geoworldsim {
namespace skill {


class MoveSkill : public Skill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveSkill();
    ~MoveSkill();

    // PROPERTIES
    static QString MAX_SPEED;
    static QString INSTANT_SPEED;
    static QString MOVING_TOWARDS;
    static QString INSTANT_DISTANCE;
    static QString ACCUMULATED_DISTANCE;
    static QString INSTANT_TIME;
    static QString ACCUMULATED_TIME;

    // GETTERS
    //GWSSpeedUnit getCurrentSpeed();
    //GWSCoordinate getMovingTowardsCoordinate();

    // SETTERS
    //void setMaxSpeed( GWSSpeedUnit max_speed ); // In m/s

    // METHODS
    virtual unit::SpeedUnit calculateNewSpeed( const unit::SpeedUnit &current_speed , const unit::SpeedUnit &max_speed , double force = 0 ); // force[0,1) Accelerate / force(-1,0] Brake
    virtual void move( const unit::TimeUnit &movement_duration , const unit::SpeedUnit &movement_speed , const geometry::Geometry &movement_towards_geom );

private:


};


}
}

#endif // MOVESKILL_H
