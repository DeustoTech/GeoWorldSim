#ifndef COMPAREPROPERTYBEHAVIOUR_H
#define COMPAREPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CompareAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CompareAgentPropertyBehaviour();

    // PROPERTIES
    static QString AGENT_PROPERTY_TO_COMPARE;
    static QString COMPARISON_OPERATOR;
    static QString COMPARISON_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // COMPAREPROPERTYBEHAVIOUR_H
