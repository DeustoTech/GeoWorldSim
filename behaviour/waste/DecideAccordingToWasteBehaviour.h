#ifndef CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H
#define CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H

#include "../Behaviour.h"

class DecideAccordingToWasteBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit DecideAccordingToWasteBehaviour();

    // GETTERS
    virtual bool continueToNext();

    virtual bool behave();
};

#endif // CHECKGENERATEDWASTEAMOUNTBEHAVIOUR_H
