#ifndef SETPROPERTYBEHAVIOUR_H
#define SETPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class SetPropertyBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetPropertyBehaviour();

    //- PROPERTIES
    static QString INPUT_ENTITY_ID;
    static QString INPUT_ENTITY_TYPE;
    static QString INPUT_PROPERTIES;
    static QString NEXTS;

protected:
     virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // SETPROPERTYBEHAVIOUR_H
