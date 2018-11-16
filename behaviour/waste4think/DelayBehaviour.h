#ifndef DELAYBEHAVIOUR_H
#define DELAYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DelayBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:

    Q_INVOKABLE explicit DelayBehaviour();

    // PROPERTIES
    static GWSTimeUnit DELAY;
    static QString NEXTS;

protected slots:
    virtual QJsonArray behave();
};

#endif // DELAYBEHAVIOUR_H
