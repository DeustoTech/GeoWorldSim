#ifndef COMPAREPROPERTYBEHAVIOUR_H
#define COMPAREPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class ComparePropertyBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ComparePropertyBehaviour();

    // PROPERTIES
    static QString ENTITY_ID;
    static QString ENTITY_TYPE;
    static QString VALUE_1;
    static QString COMPARISON_OPERATOR;
    static QString VALUE_2;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // COMPAREPROPERTYBEHAVIOUR_H
