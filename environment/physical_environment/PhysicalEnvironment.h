#ifndef GWSPHYSICALENVIRONMENT_H
#define GWSPHYSICALENVIRONMENT_H

#include <QMutex>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "../../util/geometry/NewGeometry.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Quadtree.h"

#include "../../environment/Environment.h"

using namespace QtConcurrent;

class GWSPhysicalEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSPhysicalEnvironment* globalInstance();

    // PROPERTIES
    static QString GEOMETRY_PROP;

    // SPATIAL GETTERS
    const GWSNewGeometry getBounds() const;
    GWSCoordinate getRandomCoordinate() const;
    const GWSNewGeometry getGeometry( QSharedPointer<GWSAgent> agent ) const;
    const GWSNewGeometry getGeometry( QString agent_id ) const;
    QStringList getAgentsInsideBounds( double minX , double maxX , double minY , double maxY , QString class_name ) const;
    QStringList getAgentsIntersecting( const GWSNewGeometry geometry, QString class_name ) const;
    QString getNearestAgent( GWSCoordinate coor, QString class_name ) const;
    QSharedPointer<GWSAgent> getNearestAgent( GWSCoordinate coor, QList< QSharedPointer<GWSAgent> > agents ) const;
    QStringList getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;

    // SETTERS
    void setBounds( GWSNewGeometry geom );

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

protected:

    void registerAgentToIndex( QSharedPointer<GWSAgent> agent , GWSNewGeometry geom );

protected slots:
    void agentPropertyChanged( QString property_name );

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // PHYSICAL ENVIRONMENT BOUNDS
    GWSNewGeometry environment_bounds;

    // SPATIAL INDEX
    QMap<QString , QSharedPointer< GWSQuadtree > > environment_agent_indexes; // Spatial indexes

    // Agent geometries
    QStringList agent_ids;

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSPHYSICALENVIRONMENT_H
