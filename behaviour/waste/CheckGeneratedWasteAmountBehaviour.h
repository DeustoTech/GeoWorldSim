#ifndef CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H
#define CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H

#include "../Behaviour.h"

class CheckGeneratedWasteAmountBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CheckGeneratedWasteAmountBehaviour();

    // GETTERS
    virtual bool finished();

    virtual bool behave();
};

#endif // CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H
