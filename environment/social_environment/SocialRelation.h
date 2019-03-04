#ifndef GWSSOCIALRELATION_H
#define GWSSOCIALRELATION_H

#include "../../util/graph/Edge.h"

class GWSSocialRelation : public GWSEdge
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSSocialRelation();
    ~GWSSocialRelation();

    // PROPERTIES
    static QString EDGE_FROM_AGENT_PROP;
    static QString EDGE_TO_AGENT_PROP;

private:
    QString relation_type;
};

#endif // GWSSOCIALRELATION_H
