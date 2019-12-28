#ifndef POLLUTEBEHAVIOUR_H
#define POLLUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

#include "../../util/geometry/Coordinate.h"

namespace geoworldsim {
namespace behaviour {


class PolluteBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit PolluteBehaviour();

    // PARAMETERS
    static QString INPUT_VEHICLE_SUBTYPE;
    static QString INPUT_VEHICLE_TYPE;
    static QString INPUT_POLLUTANT_TYPES;
    static QString INPUT_CURRENT_ROAD_GRADIENT;
    static QString INPUT_CURRENT_ROAD_TYPE;
    static QString INPUT_CURRENT_ROAD_TRAFFIC_SITUATION;
    static QString OUTPUT_POLLUTANTS;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();

protected:
    virtual void afterCreateHook();

private:
    geometry::Coordinate last_position;

};

}
}

#endif // POLLUTEBEHAVIOUR_H
