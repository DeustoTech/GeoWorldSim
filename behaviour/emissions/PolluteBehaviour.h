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
    static QString VEHICLE_TYPE;
    static QString TRANSPORT_MODE;
    static QString ABATEMENT_TYPE;
    static QString POLLUTANT_TYPE;
    static QString CURRENT_ROAD_GRADIENT;
    static QString CURRENT_ROAD_TYPE;
    static QString CURRENT_ROAD_TRAFFIC_SITUATION;
    static QString STORE_POLLUTANT_VALUE_AS;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();

protected:
    virtual void initialize();

private:
    GWSCoordinate last_position;

};

#endif // POLLUTEBEHAVIOUR_H
