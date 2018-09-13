#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();

};

#endif // MOVEBEHAVIOUR_H

