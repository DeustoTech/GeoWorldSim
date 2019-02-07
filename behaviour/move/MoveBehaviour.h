#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveBehaviour();

    // PROPERTIES
    static QString AGENT_MAX_SPEED;
    static QString AGENT_CURRENT_SPEED;
    static QString AGENT_MOVE_TO_X_VALUE;
    static QString AGENT_MOVE_TO_Y_VALUE;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;

private slots:
    virtual QJsonArray behave();

};

#endif // MOVEBEHAVIOUR_H

