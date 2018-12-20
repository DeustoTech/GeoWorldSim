#ifndef GWSSOCIALEDGE_H
#define GWSSOCIALEDGE_H

#include "../../util/graph/Edge.h"

class GWSSocialEdge : public GWSEdge
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSSocialEdge();
    ~GWSSocialEdge();

    // PROPERTIES
    static QString EDGE_FROM_AGENT_PROP;
    static QString EDGE_TO_AGENT_PROP;

private:
    QString relation;

};

#endif // GWSSOCIALEDGE_H
