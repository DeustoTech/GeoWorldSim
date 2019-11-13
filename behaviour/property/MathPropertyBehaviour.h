#ifndef MATHENTITYPROPERTYBEHAVIOUR_H
#define MATHENTITYPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class MathPropertyBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MathPropertyBehaviour();


    // PROPERTIES
    static QString ENTITY_ID;
    static QString ENTITY_TYPE;
    static QString OPERAND_1;
    static QString OPERAND_2;
    static QString OPERATOR;
    static QString STORE_AS;
    static QString NEXTS;

protected slots:

     virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // MATHENTITYPROPERTYBEHAVIOUR_H
