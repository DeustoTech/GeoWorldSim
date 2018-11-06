#ifndef DRIVESKILL_H
#define DRIVESKILL_H

#include "../../skill/Skill.h"
#include "../../util/units/Units.h"

class DriveSkill : public GWSSkill
{
    Q_OBJECT

public:
    DriveSkill();

    // PROPERTIES
    static QString ADJUST_TO_SPEED_LIMITS_PROP;
    static QString STOP_DISTANCE_PROP;

    // GETTERS
    double getSpeedFactor() const;
    GWSLengthUnit getStopDistanceFactor() const;

    // SETTERS
    void setSpeedFactor(double factor);
    void setStopDistance(GWSLengthUnit distance);

    // METHODS
    double calculateAccelerateForce( GWSSpeedUnit vehicle_speed , GWSSpeedUnit road_max_speed , GWSLengthUnit free_distance );


};

#endif // DRIVESKILL_H
