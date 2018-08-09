#ifndef SELECTDESTINATION_H
#define SELECTDESTINATION_H

#include "../Behaviour.h"


class SelectDestinationBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:

    Q_INVOKABLE explicit SelectDestinationBehaviour( GWSAgent* behaving_agent = Q_NULLPTR );

    // GETTERS
    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // SELECTDESTINATION_H
