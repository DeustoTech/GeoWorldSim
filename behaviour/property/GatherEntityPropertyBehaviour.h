#ifndef GATHERAGENTPROPERTYBEHAVIOUR_H
#define GATHERAGENTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GatherAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GatherAgentPropertyBehaviour();

    // PROPERTIES
    static QString PROPERTY_TO_GATHER;
    static QString GENERATING_AGENT_ID;
    static QString NEXTS;


protected slots:
     virtual QPair< double , QJsonArray > behave();


};

#endif // GATHERAGENTPROPERTYBEHAVIOUR_H
