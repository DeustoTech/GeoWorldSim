#ifndef CHECKPROPERTYVALUEBEHAVIOUR_H
#define CHECKPROPERTYVALUEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {


class CheckPropertyValueBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit CheckPropertyValueBehaviour();

    // PROPERTIES

    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;
    static QString PROPERTY_TO_COMPARE;

protected slots:
     virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // CHECKPROPERTYVALUEBEHAVIOUR_H
