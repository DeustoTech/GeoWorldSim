#ifndef POLLUTEBEHAVIOUR_H
#define POLLUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class PolluteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit PolluteBehaviour();

    // PARAMETERS
    static QString VEHICLE_TYPE;
    static QString VEHICLE_SUBTYPE;
    static QString POLLUTANT_TYPE;
    static QString STORE_POLLUTANT_VALUE_AS;
    static QString NEXTS;

protected slots:
    virtual QStringList behave();


};

#endif // POLLUTEBEHAVIOUR_H
