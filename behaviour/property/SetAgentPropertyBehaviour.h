#ifndef SETAGENTPROPERTYBEHAVIOUR_H
#define SETAGENTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetAgentPropertyBehaviour();

    virtual bool continueToNext();

    //- PROPERTIES
    static QString PROPERTY_NAME;
    static QString PROPERTY_VALUE;

protected:
    virtual bool behave();
};

#endif // SETAGENTPROPERTYBEHAVIOUR_H
