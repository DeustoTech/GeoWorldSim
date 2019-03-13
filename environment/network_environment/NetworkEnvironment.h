#ifndef GWSNETWORKENVIRONMENT_H
#define GWSNETWORKENVIRONMENT_H

#include <QObject>
#include <QReadWriteLock>

#include "../../environment/Environment.h"
#include "NetworkEdge.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/routing/Routing.h"

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
    QSharedPointer< GWSNetworkEdge > getEdge( QSharedPointer< GWSAgent > agent ) const;
    QSharedPointer< GWSAgent > getAgent( QSharedPointer< GWSNetworkEdge > edge ) const;
    QSharedPointer< GWSNetworkEdge > getEdge( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    QSharedPointer< GWSNetworkEdge > getNearestEdge( GWSCoordinate coor , QString class_name ) const;

    QList< QSharedPointer<GWSNetworkEdge> > getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const;
    QList< QList<QSharedPointer<GWSNetworkEdge> > > getShortestPath(QList< GWSCoordinate > ordered_coors , QString class_name ) const;
    QList< QList<QSharedPointer< GWSNetworkEdge> > > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const;
    QPair< GWSCoordinate , QList<QSharedPointer< GWSNetworkEdge> > > getNearestNodeAndPath( GWSCoordinate coor , QList< GWSCoordinate > get_nearest , QString class_name ) const;

    // SETTERS

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );
    QString getNearestNodeID( GWSCoordinate coor , QString class_name ) const;

private:
    GWSNetworkEnvironment();
    GWSNetworkEnvironment(GWSNetworkEnvironment const&);
    ~GWSNetworkEnvironment();

    QMap< QSharedPointer< GWSAgent > , QSharedPointer< GWSNetworkEdge > > agent_to_edge;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
