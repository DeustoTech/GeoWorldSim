#ifndef FINDCLOSESTBEHAVIOUR_H
#define FINDCLOSESTBEHAVIOUR_H


#include "../Behaviour.h"
#include "../../skill/move/MoveSkill.h"

class FindClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit FindClosestBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();
};

#endif // FINDCLOSESTBEHAVIOUR_H
