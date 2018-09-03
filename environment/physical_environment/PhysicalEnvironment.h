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
    QSharedPointer<GWSGeometry> getBounds() const;
    GWSCoordinate getRandomCoordinate() const;
    QSharedPointer<GWSGeometry> getGeometry( QSharedPointer<GWSAgent> agent ) const;
    QList< QSharedPointer<GWSAgent> > orderByDistance( QSharedPointer<GWSAgent> source , QList< QSharedPointer<GWSAgent> > agents ) const;
    QList< QSharedPointer<GWSAgent> > getAgentsInsideBounds( double minX , double maxX , double minY , double maxY , QString class_name ) const;
    QList< QSharedPointer<GWSAgent> > getAgentsIntersecting( const QSharedPointer<GWSGeometry> geometry, QString class_name ) const;
    QSharedPointer<GWSAgent> getNearestAgent( GWSCoordinate coor, QString class_name ) const;
    QSharedPointer<GWSAgent> getNearestAgent( GWSCoordinate coor, QList< QSharedPointer<GWSAgent> > agents ) const;
    QList< QSharedPointer<GWSAgent> > getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;
    QList< QSharedPointer<GWSAgent> > getNearestAgents( QList<GWSCoordinate> coors, QList< QSharedPointer<GWSAgent> > agents ) const;
    //QSharedPointer<GWSAgent> getAgentByGeometry( QSharedPointer<GWSGeometry> geometry, QString class_name ) const;

    // SETTERS
    void setBounds( QSharedPointer<GWSGeometry> geom );
    //bool updateAgentGeometry( QSharedPointer<GWSAgent> agent , GeoCoordinates new_geom );
    //bool updateAgentGeometry( QSharedPointer<GWSAgent> agent , QSharedPointer<GWSGeometry> new_geom );

    // SPATIAL OPERATIONS
    void transformMove( QSharedPointer<GWSAgent> agent, const GWSCoordinate &apply_movement );
    void transformBuffer(  QSharedPointer<GWSAgent> agent, double threshold );
    void transformUnion(  QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other );
    void transformIntersection(  QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other );

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent , QSharedPointer<GWSGeometry> init_geom = QSharedPointer<GWSGeometry>() );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // PHYSICAL ENVIRONMENT BOUNDS
    QSharedPointer<GWSGeometry> environment_bounds;

    // SPATIAL INDEX
    QMap<QString , GWSQuadtree*> spatial_index; // Spatial indexes

    // Agent geometries
    QStringList agent_ids;
    QMap<QString , QSharedPointer<GWSGeometry> > agent_geometries;

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSPHYSICALENVIRONMENT_H
