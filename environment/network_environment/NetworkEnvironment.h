#ifndef NETWORKENVIRONMENT_H
#define NETWORKENVIRONMENT_H

#include <QObject>
#include <QMutex>

#include "../../environment/Environment.h"
#include "../../util/graph/Graph.h"
#include "../../util/graph/GraphEdge.h"
#include "../../util/graph/GraphNode.h"

class GWSNetworkEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSNetworkEnvironment* globalInstance();

    // EXPORTERS
    QJsonObject serialize() const;
    void deserialize(QJsonObject json);

    // GETTERS
    GWSGraphNode* getNodeFromGraph( GeoCoordinates coor , QString class_name ) const;
    template <class T> T* getNodeFromGraph( GeoCoordinates coor , QString class_name ) const;

    GWSGraphNode* getNearestNodeFromGraph( GeoCoordinates coor , QString class_name ) const;

    const GWSGraphEdge* getEdgeFromGraph( GeoCoordinates from , GeoCoordinates to , QString class_name ) const;

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

#endif // NETWORKENVIRONMENT_H
