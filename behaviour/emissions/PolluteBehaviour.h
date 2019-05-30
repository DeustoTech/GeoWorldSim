#ifndef POLLUTEBEHAVIOUR_H
#define POLLUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

#include "../../util/geometry/Coordinate.h"

class PolluteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit PolluteBehaviour();

    // PARAMETERS
    static QString INPUT_VEHICLE_SUBTYPE;
    static QString INPUT_VEHICLE_TYPE;
    static QString ABATEMENT_TYPE;
    static QString INPUT_POLLUTANT_TYPES;
    static QString CURRENT_ROAD_GRADIENT;
    static QString CURRENT_ROAD_TYPE;
    static QString CURRENT_ROAD_TRAFFIC_SITUATION;
    static QString OUTPUT_POLLUTANTS;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();

protected:
    virtual void afterCreateHook();

private:
    GWSCoordinate last_position;

};

#endif // POLLUTEBEHAVIOUR_H
