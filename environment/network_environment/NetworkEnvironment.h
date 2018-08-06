#ifndef GWSNETWORKENVIRONMENT_H
#define GWSNETWORKENVIRONMENT_H

#include <QObject>
#include <QMutex>

#include "../../environment/Environment.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/graph/Graph.h"
#include "../../util/graph/GraphEdge.h"
#include "../../util/graph/GraphNode.h"

class GWSNetworkEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSNetworkEnvironment* globalInstance();

    // EXPORTERS
    virtual QJsonObject serialize() const;
    void deserialize(QJsonObject json);

    // GETTERS
    const GWSGraphNode* getNodeFromGraph( GWSCoordinate point , QString class_name ) const;
    template <class T> const T* getNodeFromGraph( GWSCoordinate point , QString class_name ) const;

    const GWSGraphNode* getNearestNodeFromGraph( GWSCoordinate point , QString class_name ) const;

    const GWSGraphEdge* getEdgeFromGraph( GWSCoordinate from_point , GWSCoordinate to_point , QString class_name ) const;

    const GWSGraph* getGraph( QString class_name ) const;

    // SETTERS

protected:
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    QMap<QString, GWSGraph*> network_graphs; // Graphs

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
