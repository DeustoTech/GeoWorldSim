#ifndef SETHOMEBEHAVIOUR_H
#define SETHOMEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetHomeBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetHomeBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();
};

#endif // SETHOMEBEHAVIOUR_H
