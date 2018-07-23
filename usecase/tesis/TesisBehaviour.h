#ifndef TESISBEHAVIOUR_H
#define TESISBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class TesisBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    TesisBehaviour( GWSAgent* behaving_agent );

    // PROPERTIES
    static QString COUTDOWN_PROP;

    virtual bool finished();
    bool behave();
};

#endif // TESISBEHAVIOUR_H
