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
    //GWSNetworkEdge getEdge( QSharedPointer< GWSAgent > agent ) const;
    //QSharedPointer< GWSAgent > getAgent( GWSNetworkEdge edge ) const;
    QString getEdge( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    QString getNearestAgent( GWSCoordinate coor , QString class_name ) const;

    QStringList getShortestPath( GWSCoordinate from , GWSCoordinate to , QString class_name ) const;
    QList< QStringList > getShortestPath( QList< GWSCoordinate > ordered_coors , QString class_name ) const;
    QList< QStringList > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const;
    QPair< GWSCoordinate , QList< QSharedPointer<GWSAgent> > > getNearestNodeAndPath( GWSCoordinate coor , QList< GWSCoordinate > get_nearest , QString class_name ) const;

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

protected:

    void upsertAgentToIndex( QSharedPointer<GWSAgent> agent , GWSNetworkEdge edge );

protected slots:

    void agentPropertyChanged( QString property_name );

private :

    GWSNetworkEnvironment();
    GWSNetworkEnvironment( GWSNetworkEnvironment const& );
    ~GWSNetworkEnvironment();

    // Methods
    QString getNearestNodeUID( GWSCoordinate coor , QString class_name ) const;

    // Storages
    QStringList environment_agent_index_types;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
