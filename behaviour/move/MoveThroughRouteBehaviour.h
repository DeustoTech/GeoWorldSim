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
    static QString SET_DESTINATION_X_VALUE;
    static QString SET_DESTINATION_Y_VALUE;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;
    static QString SET_NETWORK_TYPE;

protected slots:
    virtual QJsonArray behave();

protected:
    virtual void initialize();

};

#endif // GWSMOVETHROUGHROUTEBEHAVIOUR_H

