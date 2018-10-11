#ifndef CHECKIFATCLOSESTEDGEBEHAVIOUR_H
#define CHECKIFATCLOSESTEDGEBEHAVIOUR_H


#include "../../behaviour/Behaviour.h"

class CheckIfAtClosestEdgePointBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfAtClosestEdgePointBehaviour();

    virtual bool canContinueToNext();

    private slots:
    virtual bool behave();
};

#endif // CHECKIFATCLOSESTEDGEBEHAVIOUR_H
