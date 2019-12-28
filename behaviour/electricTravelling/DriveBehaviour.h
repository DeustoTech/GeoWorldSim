#ifndef DRIVEBEHAVIOUR_H
#define DRIVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DriveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit DriveBehaviour();
    ~DriveBehaviour();


    // PROPERTIES
    static QString SPEED_FACTOR_PROP;
    static QString SECURITY_DISTANCE;
    static QString VEHICLE_TYPE;
    static QString VEHICLE_SUBTYPE;
    static QString VEHICLE_LOAD;
    static QString NEXTS_IF_MOVE;
    static QString NEXTS_IF_STOP;


protected slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // DRIVEBEHAVIOUR_H
