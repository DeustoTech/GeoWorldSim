#ifndef SETENTITYTPROPERTYBEHAVIOUR_H
#define SETENTITYTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetEntityPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetEntityPropertyBehaviour();

    //- PROPERTIES
    static QString ENTITY_ID;
    static QString PROPERTIES;
    static QString NEXTS;

protected:
     virtual QPair< double , QJsonArray > behave();
};

#endif // SETENTITYTPROPERTYBEHAVIOUR_H
