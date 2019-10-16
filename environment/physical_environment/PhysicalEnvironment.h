#ifndef PHYSICALENVIRONMENT_H
#define PHYSICALENVIRONMENT_H

#include <QMutex>

#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Quadtree.h"

#include "../../environment/Environment.h"

namespace geoworldsim {
namespace environment {


class PhysicalEnvironment : public Environment
{
    Q_OBJECT

public:
    static PhysicalEnvironment* globalInstance();

    // PROPERTIES
    static QString GEOMETRY_PROP;

    // SPATIAL GETTERS
    const geometry::Geometry getBounds() const;
    const geometry::Geometry getBounds( QString &class_name ) const;
    geometry::Coordinate getRandomCoordinate() const;
    const geometry::Geometry getGeometry( QSharedPointer<Entity> agent ) const;
    const geometry::Geometry getGeometry( const QString &agent_id ) const;
    QStringList getAgentsInsideBounds( double minX , double maxX , double minY , double maxY , QString class_name ) const;
    QStringList getAgentsIntersecting( const geometry::Geometry &geometry, QString class_name ) const;
    QString getNearestEntity( const geometry::Coordinate &coor, QString class_name ) const;
    QSharedPointer<Entity> getNearestEntity( const geometry::Coordinate &coor, QList< QSharedPointer<Entity> > agents ) const;
    QStringList getNearestAgents( QList<geometry::Coordinate> coors, QString class_name ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> agent );
    virtual void unregisterEntity( QSharedPointer<Entity> agent );

protected:
    void upsertEntityToIndex( QSharedPointer<Entity> agent , const geometry::Geometry &geom );

protected slots:
    void entityPropertyChanged( QString property_name );

private:
    PhysicalEnvironment();
    PhysicalEnvironment(PhysicalEnvironment const&);
    ~PhysicalEnvironment();

    // SPATIAL INDEX
    QStringList environment_entity_index_types;
    QMap< std::string , QSharedPointer< geometry::Quadtree > > environment_entity_indexes; // Spatial indexes

    // Agent geometries
    QStringList entity_ids;

    // Mutex, for avoiding concurrency
    QReadWriteLock mutex;

};

}
}

#endif // PHYSICALENVIRONMENT_H
