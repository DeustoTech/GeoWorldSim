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

    virtual GWSMassUnit pollute( QString vehicle_type , QString transport_mode , QString pollutant , GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit , GWSLengthUnit distance );

private:
    QMap< QString , GWSSvm* > models; // QMAP< POLLUTANT , SVM >;
};

#endif // POLLUTESKILL_H
