#ifndef CHECKALIVEBEHAVIOUR_H
#define CHECKALIVEBEHAVIOUR_H

#include "../Behaviour.h"

class CheckAliveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckAliveBehaviour();

    // GETTERS
    virtual bool finished();

    virtual bool behave();

};

#endif // CHECKALIVEBEHAVIOUR_H
