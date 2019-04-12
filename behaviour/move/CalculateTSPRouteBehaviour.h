#ifndef MOVEONPARTICULARWASTETSPROUTE_H
#define MOVEONPARTICULARWASTETSPROUTE_H

#include "../../behaviour/Behaviour.h"

#include "../../util/geometry/Coordinate.h"

class CalculateTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CalculateTSPRouteBehaviour();

    // PARAMETERS
    static QString SET_TSP_AGENT_TYPE;
    static QString SET_NETWORK_TYPE;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // MOVEONPARTICULARWASTETSPROUTE_H
