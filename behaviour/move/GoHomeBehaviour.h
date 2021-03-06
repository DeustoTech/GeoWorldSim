#ifndef GOHOMEBEHAVIOUR_H
#define GOHOMEBEHAVIOUR_H

#include "../Behaviour.h"

class GoHomeBehaviour : GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit GoHomeBehaviour();

    // GETTERS
    virtual bool canContinueToNext();

private slots:
    virtual QStringList behave();
};

#endif // GOHOMEBEHAVIOUR_H
