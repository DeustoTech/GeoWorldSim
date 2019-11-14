#ifndef ACCUMULATEPROPERTYBEHAVIOUR_H
#define ACCUMULATEPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class AccumulatePropertyBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit AccumulatePropertyBehaviour();

    // PROPERTIES
    static QString ENTITY_ID;
    static QString ENTITY_TYPE;
    static QString PROPERTIES; // Object with property name and start value { "accumulated_1" : 0 , "duration_of_process" : 10 }
    static QString NEXTS;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // ACCUMULATEPROPERTYBEHAVIOUR_H
