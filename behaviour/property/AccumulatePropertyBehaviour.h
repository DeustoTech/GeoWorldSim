#ifndef ACCUMULATEPROPERTYBEHAVIOUR_H
#define ACCUMULATEPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../object/ObjectFactory.h"

class AccumulatePropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit AccumulatePropertyBehaviour();

    // PROPERTIES
    static QString ENTITY_ID;
    static QString PROPERTIES;
    static QString NEXTS;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // ACCUMULATEPROPERTYBEHAVIOUR_H
