#ifndef FOLLOWTSPROUTEBEHAVIOUR_H
#define FOLLOWTSPROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class FollowTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FollowTSPRouteBehaviour();

    // PARAMETERS:
    static QString TSP_ROUTE;
    static QString TSP_ROUTE_STAGE;
    static QString STORE_CLOSEST_ID_AS;
    static QString STORE_TSP_ROUTE_STAGE_X_AS;
    static QString STORE_TSP_ROUTE_STAGE_Y_AS;
    static QString NEXTS;

protected slots:
    virtual QJsonArray behave();
};

#endif // FOLLOWTSPROUTEBEHAVIOUR_H
