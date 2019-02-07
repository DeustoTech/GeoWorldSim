#ifndef GWSMOVETHROUGHROUTEBEHAVIOUR_H
#define GWSMOVETHROUGHROUTEBEHAVIOUR_H

#include "../../behaviour/move/MoveBehaviour.h"

class MoveThroughRouteBehaviour : public MoveBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveThroughRouteBehaviour();
    ~MoveThroughRouteBehaviour();

    // PROPERTIES
    static QString AGENT_ROUTE_DESTINATION_X_VALUE;
    static QString AGENT_ROUTE_DESTINATION_Y_VALUE;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;
    static QString AGENT_ROUTE_NETWORK_TYPE;

protected slots:
    virtual QJsonArray behave();

protected:
    virtual void initialize();

};

#endif // GWSMOVETHROUGHROUTEBEHAVIOUR_H

