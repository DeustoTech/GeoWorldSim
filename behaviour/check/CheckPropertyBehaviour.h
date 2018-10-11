#ifndef CHECKIFPROPERTYBEHAVIOUR_H
#define CHECKIFPROPERTYBEHAVIOUR_H

#include <QObject>
#include "../Behaviour.h"

class CheckPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckPropertyBehaviour();

    // PROPERTIES
    static QString CHECK_PROPERTY_NAME_PROP;
    static QString CHECK_PROPERTY_VALUE_PROP;

    virtual bool canContinueToNext(); // Behaviour finished check

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    bool behave();

};

#endif // CHECKIFPROPERTYBEHAVIOUR_H
