#ifndef POLLUTESKILL_H
#define POLLUTESKILL_H

#include "../../util/units/Units.h"
#include "../../skill/Skill.h"

#include "../../util/svm/Svm.h"

class PolluteSkill : public GWSSkill
{
    Q_OBJECT


public:

     Q_INVOKABLE explicit PolluteSkill();
    ~PolluteSkill();


    // PROPERTIES
    static QString POLLUTANT_TYPE_PROP;
    static QString VEHICLE_TYPE_PROP;

    virtual GWSMassUnit pollute(  GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit );

private:
    GWSSvm* svm = Q_NULLPTR;
};

#endif // POLLUTESKILL_H
