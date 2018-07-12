#ifndef PHYSICALENVIRONMENT_H
#define PHYSICALENVIRONMENT_H

#include <QMutex>

#include "../../util/geometry/GeometryFactory.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/geometry/Quadtree.h"

#include "../../environment/Environment.h"

using namespace geos::geom;
using namespace geos::index::quadtree;

class GWSPhysicalEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSPhysicalEnvironment* globalInstance();

    // PROPERTIES
    static QString GEOMETRY_PROP;

    // SPATIAL GETTERS
    QList<GWSAgent*> orderByDistance( GWSAgent* source , QList<GWSAgent*> agents ) const;
    QList<GWSAgent*> getAgentsInsideEnvelope( const GWSEnvelope envelope, QString class_name ) const;
    QList<GWSAgent*> getAgentsIntersecting( const GWSGeometry* geometry, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QString class_name ) const;
    GWSAgent* getNearestAgent( GWSCoordinate coor, QList<GWSAgent*> agents ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QString class_name ) const;
    QList<GWSAgent*> getNearestAgents( QList<GWSCoordinate> coors, QList<GWSAgent*> agents ) const;
    GWSAgent* getAgentByGeometry( GWSGeometry* geometry, QString class_name ) const;

    // GETTERS
    GWSEnvelope getBounds() const;

    // SETTERS
    void setBounds(GWSEnvelope bounds);
    bool updateAgentGeometry( GWSAgent* agent , GWSCoordinate new_geom );
    bool updateAgentGeometry( GWSAgent* agent , GWSGeometry* new_geom );

protected:
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSPhysicalEnvironment();
    GWSPhysicalEnvironment(GWSPhysicalEnvironment const&);
    ~GWSPhysicalEnvironment();

    // ENVIRONMENT BOUNDS
    GWSEnvelope bounds;

    // SPATIAL INDEX
    QMap<QString , GWSEnvelope> spatial_envelopes; // QMAP< AGENT_ID , ENVELOPE> Spatial envelopes
    QMap<QString , GWSQuadtree*> spatial_index; // Spatial indexes

    // Mutex, for avoiding concurrency
    QMutex mutex;
};

#endif // PHYSICALENVIRONMENT_H
