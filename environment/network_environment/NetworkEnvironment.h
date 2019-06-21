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

    // GETTERS
    //GWSNetworkEdge getEdge( QSharedPointer< GWSAgent > agent ) const;
    //QSharedPointer< GWSAgent > getAgent( GWSNetworkEdge edge ) const;
    QString getEdge( const GWSCoordinate &from , const GWSCoordinate &to , const QString &class_name ) const;
    QString getNearestAgent( const GWSCoordinate &coor , const QString &class_name ) const;

    QStringList getShortestPath( const GWSCoordinate &from , const GWSCoordinate &to , const QString &class_name ) const;
    QList< QStringList > getShortestPath( QList< GWSCoordinate > ordered_coors , const QString &class_name ) const;
    QList< QStringList > getShortestPaths( const GWSCoordinate &from_one, QList< GWSCoordinate > to_many , const QString &class_name ) const;
    QPair< GWSCoordinate , QList< QSharedPointer<GWSEntity> > > getNearestNodeAndPath( const GWSCoordinate &coor , QList< GWSCoordinate > get_nearest , const QString &class_name ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> agent );
    virtual void unregisterEntity( QSharedPointer<GWSEntity> agent );

protected:

    void upsertAgentToIndex( QSharedPointer<GWSEntity> agent , const GWSNetworkEdge &edge );

protected slots:

    void entityPropertyChanged( QString property_name );

private :

    GWSNetworkEnvironment();
    GWSNetworkEnvironment( GWSNetworkEnvironment const& );
    ~GWSNetworkEnvironment();

    // Methods
    QString getNearestNodeUID( const GWSCoordinate& coor , const QString& class_name ) const;

    // Storages
    QStringList environment_entity_index_types;
    QMap<QString , QSharedPointer< GWSQuadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< GWSRouting > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

#endif // GWSNETWORKENVIRONMENT_H
