#ifndef EXCHANGEPROPERTYBEHAVIOUR_H
#define EXCHANGEPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class ExchangePropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ExchangePropertyBehaviour();

    // GETTERS
    virtual bool canContinueToNext();

private slots:
    virtual bool behave();

};

#endif // EXCHANGEPROPERTYBEHAVIOUR_H
