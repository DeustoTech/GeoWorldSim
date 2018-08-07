#ifndef EATBEHAVIOUR_H
#define EATBEHAVIOUR_H

#include "../Behaviour.h"

class EatBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit EatBehaviour( GWSAgent* behaving_agent = Q_NULLPTR );

    // PROPERTIES
    static QString EAT_AGENT_TYPE_PROP;

    // GETTERS
    virtual bool finished();
};

#endif // EATBEHAVIOUR_H
