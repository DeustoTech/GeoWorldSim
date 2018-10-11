#ifndef EMPTYWASTEBEHAVIOUR_H
#define EMPTYWASTEBEHAVIOUR_H


#include "../Behaviour.h"

class EmptyWasteBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit EmptyWasteBehaviour();

    virtual bool canContinueToNext();

private slots:
    virtual QStringList behave();
};

#endif // EMPTYWASTEBEHAVIOUR_H
