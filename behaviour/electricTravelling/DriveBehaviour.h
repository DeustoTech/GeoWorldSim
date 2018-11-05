#ifndef DRIVEBEHAVIOUR_H
#define DRIVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DriveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit DriveBehaviour();

    // PROPERTIES
    static QString SPEED_FACTOR_PROP;
    static QString VEHICLE_TYPE;
    static QString VEHICLE_SUBTYPE;
    static QString VEHICLE_LOAD;
    static QString CURRENT_SPEED;
    static QString CURRENT_ROAD;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;


protected slots:

    virtual QStringList behave();
};

#endif // DRIVEBEHAVIOUR_H
