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

    // GETTERS
    virtual bool canContinueToNext();

private slots:
    virtual bool behave();

};

#endif // MOVEBEHAVIOUR_H

