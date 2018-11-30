#ifndef INCREMENTPROPERTYBEHAVIOUR_H
#define INCREMENTPROPERTYBEHAVIOUR_H

#include "../Behaviour.h"

class IncrementPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit IncrementPropertyBehaviour();

    // PROPERTIES
    static QString PROPERTY_NAME_PROP;
    static QString INCREMENT_VALUE_PROP;
    static QString MAX_VALUE_PROP;
    static QString MIN_VALUE_PROP;
    static QString NEXTS_IF_MAX;
    static QString NEXTS_IF_MIN;
    static QString NEXTS_IF_WITHIN_THRESHOLD;

private slots:
    virtual QJsonArray behave();

};

#endif // INCREMENTPROPERTYBEHAVIOUR_H
