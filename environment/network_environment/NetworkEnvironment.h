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

    // PROPERTIES
    static QString NODE_PROP;
    static QString EDGE_PROP;

    // EXPORTERS
    virtual QJsonObject serialize() const;
    void deserialize(QJsonObject json);

    // GETTERS
    const QSharedPointer<GWSGraphNode> getNode( QSharedPointer<GWSAgent> agent ) const;
    const QSharedPointer<GWSGraphEdge> getEdge( QSharedPointer<GWSAgent> agent ) const;
    QSharedPointer<GWSGraphNode> getNodeFromGraph( GWSCoordinate point , QString class_name ) const;
    template <class T> QSharedPointer<T> getNodeFromGraph( GWSCoordinate point , QString class_name ) const;
    QSharedPointer<GWSGraphNode> getNearestNodeFromGraph( GWSCoordinate point , QString class_name ) const;
    QSharedPointer<GWSGraphEdge> getEdgeFromGraph( GWSCoordinate from_point , GWSCoordinate to_point , QString class_name ) const;

    const GWSGraph* getGraph( QString class_name ) const;

    // SETTERS

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    QMap< QSharedPointer<GWSAgent> , QSharedPointer<GWSGraphNode> > agent_to_node;
    QMap< QSharedPointer<GWSAgent> , QSharedPointer<GWSGraphEdge> > agent_to_edge;
    QMap<QString, GWSGraph*> network_graphs; // Graphs

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
