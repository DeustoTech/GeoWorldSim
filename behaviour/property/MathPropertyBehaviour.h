#ifndef MATHENTITYPROPERTYBEHAVIOUR_H
#define MATHENTITYPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MathPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MathPropertyBehaviour();


    // PROPERTIES
    static QString ENTITY_ID;
    static QString ENTITY_TYPE;
    static QString OPERATOR;
    static QString OPERAND_VALUE;
    static QString OPERAND_PROPERTY;
    static QString ENTITY_ID_TO_OPERATE_ON;
    static QString NEXTS;

protected slots:

     virtual QPair< double , QJsonArray > behave();

};

#endif // MATHENTITYPROPERTYBEHAVIOUR_H
