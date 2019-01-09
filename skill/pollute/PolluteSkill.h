#ifndef POLLUTESKILL_H
#define POLLUTESKILL_H

#include "../../util/units/Units.h"
#include "../../skill/Skill.h"

class PolluteSkill : public GWSSkill
{
    Q_OBJECT


public:

     Q_INVOKABLE explicit PolluteSkill();
    ~PolluteSkill();


    // PROPERTIES
    static QString POLLUTANT_TYPE_PROP;

    GWSMassUnit pollute(  );

    virtual GWSMassUnit pollute(  GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit );


};

#endif // POLLUTESKILL_H
