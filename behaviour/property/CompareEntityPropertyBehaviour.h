#ifndef COMPAREPROPERTYBEHAVIOUR_H
#define COMPAREPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CompareEntityPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CompareEntityPropertyBehaviour();

    // PROPERTIES
    static QString ENTITY_PROPERTY_TO_COMPARE;
    static QString COMPARISON_OPERATOR;
    static QString COMPARISON_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // COMPAREPROPERTYBEHAVIOUR_H
