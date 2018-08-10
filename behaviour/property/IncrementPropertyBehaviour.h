#ifndef INCREMENTPROPERTYBEHAVIOUR_H
#define INCREMENTPROPERTYBEHAVIOUR_H

#include "../Behaviour.h"

class IncrementPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit IncrementPropertyBehaviour( GWSAgent* behaving_agent = Q_NULLPTR );

    // PROPERTIES
    static QString PROPERTY_NAME_PROP;
    static QString INCREMENT_VALUE_PROP;
    static QString MAX_VALUE_PROP;
    static QString MIN_VALUE_PROP;

    virtual bool finished(); // Behaviour finished check

private slots:
    virtual bool behave();

};

#endif // INCREMENTPROPERTYBEHAVIOUR_H
