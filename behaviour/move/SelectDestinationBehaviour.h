#ifndef SELECTDESTINATION_H
#define SELECTDESTINATION_H

#include "../Behaviour.h"


class SelectDestinationBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:

    Q_INVOKABLE explicit SelectDestinationBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();
};

#endif // SELECTDESTINATION_H
