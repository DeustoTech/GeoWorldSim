#ifndef GWSNETWORKENVIRONMENT_H
#define GWSNETWORKENVIRONMENT_H

#include <QObject>
#include <QReadWriteLock>

#include "../../environment/Environment.h"
#include "NetworkEdge.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/routing/Routing.hpp"

class GWSNetworkEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSNetworkEnvironment* globalInstance();

    // PROPERTIES
    static QString EDGE_PROP;

    // EXPORTERS
    virtual QJsonObject serialize() const;
    void deserialize(QJsonObject json);

    // GETTERS
    GWSNetworkEdge getEdge( QSharedPointer< GWSAgent > agent ) const;
    QSharedPointer< GWSAgent > getAgent( GWSNetworkEdge edge ) const;
    GWSNetworkEdge getEdge( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    GWSNetworkEdge getNearestEdge( GWSCoordinate coor , QString class_name ) const;

    QList< GWSNetworkEdge > getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const;
    QList< QList<GWSNetworkEdge > > getShortestPath(QList< GWSCoordinate > ordered_coors , QString class_name ) const;
    QList< QList<GWSNetworkEdge > > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const;
    QPair< GWSCoordinate , QList<GWSNetworkEdge > > getNearestNodeAndPath( GWSCoordinate coor , QList< GWSCoordinate > get_nearest , QString class_name ) const;

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    // Methods
    QString getNearestNodeUID( GWSCoordinate coor , QString class_name ) const;

    // Storages
    QMap< QString , GWSNetworkEdge > edge_ids;
    QMap< QSharedPointer< GWSAgent > , GWSNetworkEdge > agent_to_edge;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting< GWSNetworkEdge > > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
