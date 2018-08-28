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
    const GWSGeometry* getGeometry( QString agent_id ) const;
    QList<GWSAgent*> orderByDistance( GWSAgent* source , QList<GWSAgent*> agents ) const;
    QList<GWSAgent*> getAgentsInsideBounds( double minX , double maxX , double minY , double maxY , QString class_name ) const;
    QList<GWSAgent*> getAgentsIntersecting( const GWSGeometry* geometry, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QList<GWSAgent*> agents ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QList<GWSAgent*> agents ) const;
    //GWSAgent* getAgentByGeometry( GWSGeometry* geometry, QString class_name ) const;

    // SETTERS
    //bool updateAgentGeometry( GWSAgent* agent , GeoCoordinates new_geom );
    //bool updateAgentGeometry( GWSAgent* agent , GWSGeometry* new_geom );

    // SPATIAL OPERATIONS
    void transformMove( GWSAgent* agent, const GWSCoordinate &apply_movement );
    void transformBuffer(  GWSAgent* agent, double threshold );
    void transformUnion(  GWSAgent* agent, const GWSGeometry* other );
    void transformIntersection(  GWSAgent* agent, const GWSGeometry* other );

    // METHODS
    virtual void registerAgent(GWSAgent *agent , GWSGeometry* init_geom = Q_NULLPTR );
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // SPATIAL INDEX
    QMap<QString , GWSQuadtree*> spatial_index; // Spatial indexes

    // Agent geometries
    QStringList agent_ids;
    QMap<QString , GWSGeometry*> agent_geometries;

    // Mutex, for avoiding concurrency
    QMutex mutex;

};

#endif // GWSPHYSICALENVIRONMENT_H
