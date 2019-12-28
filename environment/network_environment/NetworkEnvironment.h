#ifndef NETWORKENVIRONMENT_H
#define NETWORKENVIRONMENT_H

#include <QObject>
#include <QReadWriteLock>

#include "../../environment/Environment.h"
#include "NetworkEdge.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/routing/Routing.h"

namespace geoworldsim {
namespace environment {


class NetworkEnvironment : public Environment
{
    Q_OBJECT

public:
    static NetworkEnvironment* globalInstance();

    // PROPERTIES
    static QString EDGE_PROP;

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    //GWSNetworkEdge getEdge( QSharedPointer< GWSAgent > agent ) const;
    //QSharedPointer< GWSAgent > getAgent( GWSNetworkEdge edge ) const;
    QString getEdge( const geometry::Coordinate &from , const geometry::Coordinate &to , const QString &class_name ) const;
    QString getNearestAgent( const geometry::Coordinate &coor , const QString &class_name ) const;

    QStringList getShortestPath( const geometry::Coordinate &from , const geometry::Coordinate &to , const QString &class_name ) const;
    QList< QStringList > getShortestPath( QList< geometry::Coordinate > ordered_coors , const QString &class_name ) const;
    QList< QStringList > getShortestPaths( const geometry::Coordinate &from_one, QList< geometry::Coordinate > to_many , const QString &class_name ) const;
    QPair< geometry::Coordinate , QList< QSharedPointer< Entity > > > getNearestNodeAndPath( const geometry::Coordinate &coor , QList< geometry::Coordinate > get_nearest , const QString &class_name ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer< Entity > entity );
    virtual void unregisterEntity( QSharedPointer< Entity > entity );

protected:

    void upsertEntityToIndex( QSharedPointer< Entity > entity , const graph::NetworkEdge &edge );

protected slots:

    void entityPropertyChanged( QString property_name );

private :

    NetworkEnvironment();
    NetworkEnvironment( NetworkEnvironment const& );
    ~NetworkEnvironment();

    // Methods
    QString getNearestNodeUID( const geometry::Coordinate& coor , const QString& class_name ) const;

    // Storages
    QStringList environment_entity_index_types;
    QMap<QString , QSharedPointer< geometry::Quadtree > > network_edges; // Edges indexed
    QMap<QString , QSharedPointer< routing::Routing > > network_routings;

    // Mutex, for avoiding concurrency
    mutable QReadWriteLock mutex;

};

}
}

#endif // NETWORKENVIRONMENT_H
