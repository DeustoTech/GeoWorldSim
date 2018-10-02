#ifndef TIMBERFACTORYBEHAVIOUR_H
#define TIMBERFACTORYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class TimberFactoryBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TimberFactoryBehaviour();

    virtual bool behave();
};

#endif // TIMBERFACTORYBEHAVIOUR_H
