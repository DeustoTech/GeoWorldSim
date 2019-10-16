#ifndef POLLUTESKILL_H
#define POLLUTESKILL_H

#include "../../util/units/Units.h"
#include "../../skill/Skill.h"
#include "../../util/svm/Svm.h"

namespace geoworldsim {
namespace skill {

class PolluteSkill : public Skill
{
    Q_OBJECT

public:

     Q_INVOKABLE explicit PolluteSkill();
    ~PolluteSkill();

    // PROPERTIES
    static QString POLLUTANT_TYPE_PROP;
    static QString VEHICLE_TYPE_PROP;

    virtual unit::MassUnit pollute( QString transport_mode , QString vehicle_subtype , QString pollutant , unit::SpeedUnit speed , double gradient , QString roadType , double trafficSit , unit::LengthUnit distance );

private:
    QMap< QString , GWSSvm* > models; // QMAP< POLLUTANT , SVM >;
};

}
}

#endif // POLLUTESKILL_H
