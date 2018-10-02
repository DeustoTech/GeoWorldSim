#ifndef TREEFARMBEHAVIOUR_H
#define TREEFARMBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class TreeFarmBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit TreeFarmBehaviour();

    virtual bool behave();
};

#endif // TREEFARMBEHAVIOUR_H
