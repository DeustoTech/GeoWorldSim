#ifndef GWSMOVETHROUGHROUTEBEHAVIOUR_H
#define GWSMOVETHROUGHROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveThroughRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveThroughRouteBehaviour();
    ~MoveThroughRouteBehaviour();


    // PROPERTIES
    static QString MAX_SPEED_PROP;
    static QString X_VALUE;
    static QString Y_VALUE;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;
    static QString STORE_MOVED_DISTANCE_AS;
    static QString STORE_TRAVEL_TIME_AS;

protected slots:
    virtual QStringList behave();

};

#endif // GWSMOVETHROUGHROUTEBEHAVIOUR_H

