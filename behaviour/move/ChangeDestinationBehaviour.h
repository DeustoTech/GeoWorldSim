#ifndef CHANGEDESTINATIONBEHAVIOUR_H
#define CHANGEDESTINATIONBEHAVIOUR_H

#include "../Behaviour.h"

class ChangeDestinationBehaviour : public GWSBehaviour
{

    Q_OBJECT
public:

    Q_INVOKABLE explicit ChangeDestinationBehaviour( GWSAgent* behaving_agent = Q_NULLPTR );

    // GETTERS
    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // CHANGEDESTINATIONBEHAVIOUR_H
