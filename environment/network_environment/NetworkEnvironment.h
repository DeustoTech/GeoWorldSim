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

    QMap<QString, GWSGraph*> network_graphs; // Graphs

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
