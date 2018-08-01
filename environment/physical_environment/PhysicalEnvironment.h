#ifndef GWSPHYSICALENVIRONMENT_H
#define GWSPHYSICALENVIRONMENT_H

#include <QMutex>

#include "../../util/geometry/Quadtree.h"

#include "../../environment/Environment.h"

QT_FORWARD_DECLARE_CLASS(GWSGeometry)

using namespace geos::geom;

class GWSPhysicalEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSPhysicalEnvironment* globalInstance();

    // SPATIAL GETTERS
    QList<GWSAgent*> orderByDistance( GWSAgent* source , QList<GWSAgent*> agents ) const;
    //QList<GWSAgent*> getAgentsInsideEnvelope( const GWSEnvelope envelope, QString class_name ) const;
    //QList<GWSAgent*> getAgentsIntersecting( const GWSGeometry* geometry, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QList<GWSAgent*> agents ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QList<GWSAgent*> agents ) const;
    //GWSAgent* getAgentByGeometry( GWSGeometry* geometry, QString class_name ) const;

    // SETTERS
    //bool updateAgentGeometry( GWSAgent* agent , GeoCoordinates new_geom );
    //bool updateAgentGeometry( GWSAgent* agent , GWSGeometry* new_geom );

protected:
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // SPATIAL INDEX
    QMap<QString , GWSQuadtree*> spatial_index; // Spatial indexes

    // Mutex, for avoiding concurrency
    QMutex mutex;
};

#endif // GWSPHYSICALENVIRONMENT_H
