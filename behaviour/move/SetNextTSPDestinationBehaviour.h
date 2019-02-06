#ifndef SETNEXTTSPDESTINATIONBEHAVIOUR_H
#define SETNEXTTSPDESTINATIONBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetNextTSPDestinationBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetNextTSPDestinationBehaviour();

    // PARAMETERS:
    static QString TSP_ROUTE_PROPERTY_NAME;
    static QString STORE_NEXT_DESTINATION_AGENT_ID_AS;
    static QString NEXTS_WHILE_PENDING_ROUTE;
    static QString NEXTS_IF_ROUTE_FINISHED;

protected slots:
    virtual QJsonArray behave();
};

#endif // SETNEXTTSPDESTINATIONBEHAVIOUR_H
