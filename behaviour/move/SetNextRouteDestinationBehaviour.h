#ifndef SETNEXTROUTEDESTINATIONBEHAVIOUR_H
#define SETNEXTROUTEDESTINATIONBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetNextRouteDestinationBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetNextRouteDestinationBehaviour();

    // PROPERTIES
    static QString NEXTS_WHILE_PENDING_ROUTE;
    static QString NEXTS_IF_ROUTE_FINISHED;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // SETNEXTROUTEDESTINATIONBEHAVIOUR_H
