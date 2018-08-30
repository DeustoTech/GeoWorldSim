#ifndef EMPTYWASTEBEHAVIOUR_H
#define EMPTYWASTEBEHAVIOUR_H


#include "../Behaviour.h"

class EmptyWasteBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit EmptyWasteBehaviour();

    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // EMPTYWASTEBEHAVIOUR_H
