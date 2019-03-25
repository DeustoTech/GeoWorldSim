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
    GWSNewNetworkEdge getEdge( QSharedPointer< GWSAgent > agent ) const;
    QSharedPointer< GWSAgent > getAgent( GWSNewNetworkEdge edge ) const;
    GWSNewNetworkEdge getEdge( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    GWSNewNetworkEdge getNearestEdge( GWSCoordinate coor , QString class_name ) const;

    QList< GWSNewNetworkEdge > getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const;
    QList< QList<GWSNewNetworkEdge > > getShortestPath(QList< GWSCoordinate > ordered_coors , QString class_name ) const;
    QList< QList<GWSNewNetworkEdge > > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const;
    QPair< GWSCoordinate , QList<GWSNewNetworkEdge > > getNearestNodeAndPath( GWSCoordinate coor , QList< GWSCoordinate > get_nearest , QString class_name ) const;

    // SETTERS

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );
    QString getNearestNodeUID( GWSCoordinate coor , QString class_name ) const;

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    // Storages
    QMap< QString , GWSNewNetworkEdge > edge_ids;
    QMap< QSharedPointer< GWSAgent > , GWSNewNetworkEdge > agent_to_edge;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting< GWSNewNetworkEdge > > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
