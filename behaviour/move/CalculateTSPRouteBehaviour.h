#ifndef GWSCALCULATETSPROUTEBEHAVIOUR_H
#define GWSCALCULATETSPROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../util/geometry/Coordinate.h"

class GWSCalculateTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSCalculateTSPRouteBehaviour();

    // PARAMETERS
    static QString TSP_AGENT_TYPE;
    static QString NETWORK_TYPE;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // GWSCALCULATETSPROUTEBEHAVIOUR_H
