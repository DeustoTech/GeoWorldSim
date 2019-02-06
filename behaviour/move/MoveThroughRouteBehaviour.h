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
    static QString BEHAVIOUR_DESTINATION_X_VALUE;
    static QString BEHAVIOUR_DESTINATION_Y_VALUE;
    static QString BEHAVIOUR_NEXTS_IF_ARRIVED;
    static QString BEHAVIOUR_NEXTS_IF_NOT_ARRIVED;
    static QString BEHAVIOUR_NETWORK_TYPE_PROP;

protected slots:
    virtual QJsonArray behave();

protected:
    virtual void initialize();

};

#endif // GWSMOVETHROUGHROUTEBEHAVIOUR_H

