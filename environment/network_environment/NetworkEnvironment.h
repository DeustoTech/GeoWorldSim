#ifndef GWSNETWORKENVIRONMENT_H
#define GWSNETWORKENVIRONMENT_H

#include <QObject>
#include <QMutex>

#include <lemon/core.h>
#include <lemon/list_graph.h>

#include "../../environment/Environment.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/graph/GraphEdge.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/routing/Routing.h"

using namespace lemon;

class GWSNetworkEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSNetworkEnvironment* globalInstance();

    // PROPERTIES
    static QString EDGE_PROP;
    static QString INSIDE_AGENT_IDS_PROP;

    // EXPORTERS
    virtual QJsonObject serialize() const;
    void deserialize(QJsonObject json);

    // GETTERS
    QSharedPointer<GWSGraphEdge> getEdge( QSharedPointer<GWSAgent> agent ) const;
    QSharedPointer<GWSAgent> getAgent( QSharedPointer<GWSGraphEdge> edge ) const;
    QSharedPointer<GWSGraphEdge> getEdge( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    GWSCoordinate getNearestNode( GWSCoordinate coor , QString class_name ) const;
    QSharedPointer<GWSGraphEdge> getNearestEdge( GWSCoordinate coor , QString class_name ) const;

    QList<QSharedPointer<GWSGraphEdge> > getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const;
    QList<QList<QSharedPointer<GWSGraphEdge> > > getShortestPath(QList< GWSCoordinate > ordered_coors , QString class_name ) const;
    QList<QList<QSharedPointer< GWSGraphEdge> > > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const;
    QPair< GWSCoordinate , QList<QSharedPointer< GWSGraphEdge> > > getNearestNodeAndPath( GWSCoordinate coor , QList< GWSCoordinate > get_nearest , QString class_name ) const;

    // SETTERS

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    QMap< QSharedPointer<GWSAgent> , QSharedPointer<GWSGraphEdge> > agent_to_edge;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QMutex mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
