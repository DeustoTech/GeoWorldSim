#ifndef GWSPHYSICALENVIRONMENT_H
#define GWSPHYSICALENVIRONMENT_H

#include <QMutex>

#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Quadtree.h"

#include "../../environment/Environment.h"

class GWSPhysicalEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSPhysicalEnvironment* globalInstance();

    // PROPERTIES
    static QString GEOMETRY_PROP;

    // SPATIAL GETTERS
    const GWSGeometry getBounds() const;
    const GWSGeometry getBounds( QString &class_name ) const;
    GWSCoordinate getRandomCoordinate() const;
    const GWSGeometry getGeometry( QSharedPointer<GWSEntity> agent ) const;
    const GWSGeometry getGeometry( const QString &agent_id ) const;
    QStringList getAgentsInsideBounds( double minX , double maxX , double minY , double maxY , QString class_name ) const;
    QStringList getAgentsIntersecting( const GWSGeometry &geometry, QString class_name ) const;
    QString getNearestAgent( const GWSCoordinate &coor, QString class_name ) const;
    QSharedPointer<GWSEntity> getNearestAgent( const GWSCoordinate &coor, QList< QSharedPointer<GWSEntity> > agents ) const;
    QStringList getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> agent );
    virtual void unregisterEntity( QSharedPointer<GWSEntity> agent );

protected:
    void upsertEntityToIndex( QSharedPointer<GWSEntity> agent , const GWSGeometry &geom );

protected slots:
    void entityPropertyChanged( QString property_name );

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // SPATIAL INDEX
    QStringList environment_entity_index_types;
    QMap< QString , QSharedPointer< GWSQuadtree > > environment_entity_indexes; // Spatial indexes

    // Agent geometries
    QStringList entity_ids;

    // Mutex, for avoiding concurrency
    QReadWriteLock mutex;

};

#endif // GWSPHYSICALENVIRONMENT_H
