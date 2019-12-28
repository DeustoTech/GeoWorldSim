#ifndef EATBEHAVIOUR_H
#define EATBEHAVIOUR_H

#include "../Behaviour.h"

class EatBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit EatBehaviour();

    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // EATBEHAVIOUR_H
