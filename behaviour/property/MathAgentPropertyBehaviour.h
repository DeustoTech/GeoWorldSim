#ifndef MATHAGENTPROPERTYBEHAVIOUR_H
#define MATHAGENTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MathAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MathAgentPropertyBehaviour();


    // PROPERTIES
    static QString OPERATOR;
    static QString OPERAND_VALUE;
    static QString OPERAND_PROPERTY;
    static QString AGENT_ID_TO_OPERATE_ON;
    static QString NEXTS;

protected slots:

    virtual QJsonArray behave();

};

#endif // MATHAGENTPROPERTYBEHAVIOUR_H
