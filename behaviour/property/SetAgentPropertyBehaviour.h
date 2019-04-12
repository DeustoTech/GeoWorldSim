#ifndef SETAGENTPROPERTYBEHAVIOUR_H
#define SETAGENTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetAgentPropertyBehaviour();

    //- PROPERTIES
    static QString AGENT_ID;
    static QString PROPERTY_NAME;
    static QString PROPERTY_VALUE;
    static QString NEXTS;

protected:
     virtual QPair< double , QJsonArray > behave();
};

#endif // SETAGENTPROPERTYBEHAVIOUR_H
