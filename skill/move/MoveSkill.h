#ifndef MOVESKILL_H
#define MOVESKILL_H

#include "../Skill.h"
#include "../../util/units/Units.h"
#include "../../util/geometry/Geometry.h"

class MoveSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveSkill( GWSAgent *skilled_agent = Q_NULLPTR );
    ~MoveSkill();

    // PROPERTIES
    static QString MAX_SPEED_PROP;
    static QString CURRENT_SPEED_PROP;
    static QString ACCUMULATED_DISTANCE_PROP;
    static QString ACCUMULATED_TIME_PROP;

    // GETTERS
    GWSSpeedUnit getMaxSpeed() const;
    GWSSpeedUnit getCurrentSpeed() const;
    GWSLengthUnit getAccDistance() const;
    GWSTimeUnit getAccTime() const;

    // SETTERS
    void setMaxSpeed( GWSSpeedUnit max_speed ); // In m/s

    // METHODS
    virtual GWSSpeedUnit changeSpeed( double force = 0 ); // force[0,1) Accelerate / force(-1,0] Brake
    virtual GWSSpeedUnit brakeToStop();
    virtual void moveTowards( GWSCoordinate coor , GWSTimeUnit movement_duration );

signals:
    void speedChangedSignal( GWSSpeedUnit current_speed );
    void movedSignal();

private:


};

#endif // MOVESKILL_H