#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveBehaviour();

    // PROPERTIES
    static QString MAX_SPEED_PROP;
    static QString X_VALUE;
    static QString Y_VALUE;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;

private slots:
    virtual QJsonArray behave();

};

#endif // MOVEBEHAVIOUR_H

