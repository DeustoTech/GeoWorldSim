#ifndef VEHICLEPOLLUTESKILL_H
#define VEHICLEPOLLUTESKILL_H

#include "PolluteSkill.h"
#include "../../util/svm/Svm.h"

class VehiclePolluteSkill : public PolluteSkill
{
    Q_OBJECT

public:

     Q_INVOKABLE explicit  VehiclePolluteSkill();
    ~VehiclePolluteSkill();

    // PROPERTIES
    static QString VEHICLE_TYPE_PROP;

    virtual GWSMassUnit pollute(  GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit );

private:
    GWSSvm* svm = new GWSSvm();
    svm_model *model = Q_NULLPTR;
};

#endif // VEHICLEPOLLUTESKILL_H
