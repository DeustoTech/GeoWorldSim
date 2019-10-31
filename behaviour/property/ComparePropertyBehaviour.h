#ifndef COMPAREPROPERTYBEHAVIOUR_H
#define COMPAREPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class ComparePropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ComparePropertyBehaviour();

    // PROPERTIES
    static QString ENTITY_ID;
    static QString ENTITY_TYPE;
    static QString COMPARE_PROPERTY;
    static QString COMPARISON_OPERATOR;
    static QString COMPARISON_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // COMPAREPROPERTYBEHAVIOUR_H
