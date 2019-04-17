#ifndef INCREMENTPROPERTYBEHAVIOUR_H
#define INCREMENTPROPERTYBEHAVIOUR_H

#include "../Behaviour.h"

class IncrementPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit IncrementPropertyBehaviour();

    // PROPERTIES
    static QString AGENT_ID;
    static QString PROPERTY_NAME_PROP;
    static QString INCREMENT_VALUE_PROP;
    static QString MAX_THRESHOLD_VALUE_PROP;
    static QString MIN_THRESHOLD_VALUE_PROP;
    static QString MAX_LIMIT_VALUE_PROP;
    static QString MIN_LIMIT_VALUE_PROP;
    static QString NEXTS_IF_MAX_THRESHOLD_REACHED;
    static QString NEXTS_IF_MIN_THRESHOLD_REACHED;
    static QString NEXTS_ELSE;

private slots:
     virtual QPair< double , QJsonArray > behave();

};

#endif // INCREMENTPROPERTYBEHAVIOUR_H
