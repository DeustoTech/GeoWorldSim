#ifndef CHECKIFATPOSITION_H
#define CHECKIFATPOSITION_H

#include "../../behaviour/Behaviour.h"

class CheckIfAtPositionBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfAtPositionBehaviour();

    // PROPERTIES
    static QString KEY_POSITION_X;
    static QString KEY_POSITION_Y;

    // GETTERS
    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // CHECKIFATPOSITION_H
