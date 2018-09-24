#ifndef MOVEALONGSTAGEDROUTEBEHAVIOUR_H
#define MOVEALONGSTAGEDROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"




class MoveAlongStagedRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveAlongStagedRouteBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();


};

#endif // MOVEALONGSTAGEDROUTEBEHAVIOUR_H
