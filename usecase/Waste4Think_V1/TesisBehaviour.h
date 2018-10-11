#ifndef TESISBEHAVIOUR_H
#define TESISBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class TesisBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    TesisBehaviour();

    // PROPERTIES
    static QString COUTDOWN_PROP;

    virtual bool canContinueToNext();
    bool behave();
};

#endif // TESISBEHAVIOUR_H
