#ifndef DRIVESKILL_H
#define DRIVESKILL_H

#include "skills/Skill.h"

class DriveSkill : public GWSSkill
{
    Q_OBJECT

public:
    DriveSkill(GWSAgent* parent);

    // PROPERTIES
    static QString SPEED_FACTOR_PROP;
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