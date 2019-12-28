#ifndef MOVETHROUGHNETWORKBEHAVIOUR_H
#define MOVETHROUGHNETWORKBEHAVIOUR_H

#include "../../behaviour/move/MoveBehaviour.h"

class MoveThroughNetworkBehaviour : public MoveBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveThroughNetworkBehaviour();
    ~MoveThroughNetworkBehaviour();

    // PROPERTIES
    static QString INPUT_ROUTE_DESTINATION;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;
    static QString INPUT_TRANSPORT_NETWORK_TYPE;

protected slots:
    virtual QPair< double , QJsonArray > behave();

protected:
    virtual void afterCreateHook();

};

#endif // MOVETHROUGHNETWORKBEHAVIOUR_H

