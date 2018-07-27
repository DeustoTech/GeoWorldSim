
#include "PhysicalEnvironment.h"

GWSPhysicalEnvironment* GWSPhysicalEnvironment::globalInstance(){
    static GWSPhysicalEnvironment instance;
    return &instance;
}

GWSPhysicalEnvironment::GWSPhysicalEnvironment() : GWSEnvironment(){
    qInfo() << "PhysicalEnvironment created";
}

GWSPhysicalEnvironment::~GWSPhysicalEnvironment(){
    qDeleteAll(this->spatial_index.values());
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

QList<GWSAgent*> GWSPhysicalEnvironment::orderByDistance(GWSAgent* source, QList<GWSAgent*> agents) const{
    QList<GWSAgent*> ordered;
    /*if( !agents.size() ){
        return ordered;
    }

     while( agents.size() ){
        GWSAgent* nearest = agents.at( 0 );
        const GWSGeometry* nearest_geom = nearest->getGeometry();

        foreach( GWSAgent* agent , agents ){
            const GWSGeometry* agent_geom = agent->getGeometry();

            if( source->getGeometry()->getDistance( agent_geom ) < source->getGeometry()->getDistance( nearest_geom ) ){
                nearest = agent;
                nearest_geom = agent_geom;
            }
        }

        ordered.append( nearest );
        agents.removeOne( nearest );
    }*/

    return ordered;
}


/*QList<GWSAgent*> GWSPhysicalEnvironment::getAgentsInsideEnvelope(const GWSEnvelope envelope, QString class_name) const{

    QList<GWSAgent*> agents;

    if( this->spatial_index.keys().contains(class_name) ){
        foreach( void * o , this->spatial_index[class_name]->getElements( envelope ) ){
            GWSAgent* agent = ((GWSAgent*)o);
            if( envelope.contains( agent->getGeometry()->getEnvelope() ) ){
                agents.append( agent );
            }
        }
    }

    return agents;
}*/


/*QList<GWSAgent*> GWSPhysicalEnvironment::getAgentsIntersecting(const GWSGeometry* geometry, QString class_name) const{
    QList<GWSAgent*> agents;

    // Precisely compare inside geom
    foreach(GWSAgent* agent , this->getAgentsInsideEnvelope( geometry->getEnvelope() , class_name ) ){
        if( geometry->intersects( agent->getGeometry() ) ){
            agents.append( agent );
        }
    }
    return agents;
}*/

/**
 * @brief PhysicalEnvironment::getNearestAgents Given a list of geometries and the class_name of agents in the
 * environment, returns an ordered list of the nearest agent for each geometry.
 * IMPORTANT, if an agent is not snapped it will return a 0 agent to preserve list size and order.
 * @param geometries List of geometries from where to start searching
 * @param class_name ClassName of the agents in the environment from which to get the nearest for each geometry
 * @return
 */
QList<GWSAgent*> GWSPhysicalEnvironment::getNearestAgents(QList<GWSCoordinate> coors, QString class_name) const{
    QList<GWSAgent*> founds = QList<GWSAgent*>();
    foreach(GWSCoordinate coor , coors){
        founds.append( this->getNearestAgent( coor , class_name ) );
    }
    return founds;
}

GWSAgent* GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QList<GWSAgent*> agents) const{
    QList<GWSCoordinate> coors;
    coors.append( coor );
    QList<GWSAgent*> nearests = this->getNearestAgents( coors , agents );
    if( !nearests.isEmpty() ){
        return nearests.at(0);
    }
    return 0;
}

/**
 * @brief PhysicalEnvironment::getNearestAgents Given a list of geometries and the class_name of agents in the
 * environment, returns an ordered list of the nearest agent for each geometry.
 * @param geometry Geometry from where to start searching
 * @param class_name ClassName of the agents in the environment from which to get the nearest for geometry
 * @return
 */
GWSAgent* GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QString class_name) const{
    if( this->spatial_index.keys().contains(class_name) ){
        return this->spatial_index.value(class_name)->getNearestElement( coor );
    }
    return Q_NULLPTR;
}

/**
 * @brief PhysicalEnvironment::getNearestAgents Given a list of geometries and some agents to search, returns
 * an ordered list of the nearest agent for each geometry.
 * IMPORTANT, if an agent is not snapped it will return a 0 agent to preserve list size and order.
 * @param geometries List of geometries from where to start searching
 * @param agents List of agents from which to get the nearest for each geometry
 * @return
 */
QList<GWSAgent*> GWSPhysicalEnvironment::getNearestAgents(QList<GWSCoordinate> coors, QList<GWSAgent*> agents) const{
    QList<GWSAgent*> founds;

    /*GWSQuadtree* index = new GWSQuadtree();
    foreach(GWSAgent* agent , agents){
        index->insert( agent->getGeometry()->getEnvelope() , agent );
    }

    foreach(GWSCoordinate coor , coors){
        GWSAgent* found = 0;
        GWSEnvelope env = GWSEnvelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
        QList<void *> objs = index->getElements( env );

        if( !objs.isEmpty() ){
            found = ( (GWSAgent*) objs.at( qrand() % objs.size() ) );
            foreach(void * o , objs){
                GWSAgent* agent = ((GWSAgent*) o);
                if( agent && coor.distance( agent->getRepresentativeCoordinate() ) < coor.distance( found->getRepresentativeCoordinate() ) ){
                    found = agent;
                }
            }
        }
        founds.append( found );
    }

    delete index;*/
    return founds;
}


/*GWSAgent* GWSPhysicalEnvironment::getAgentByGeometry(GWSGeometry* geometry, QString class_name) const{
    if( this->spatial_index.contains(class_name) ){

        QList<void *> objs = this->spatial_index[class_name]->getElements( geometry->getEnvelope() );

        if( !objs.isEmpty() ){
            foreach(void * o , objs){
                GWSAgent* agent = ( (GWSAgent*) o);
                if( agent->getGeometry()->equals( geometry ) ){
                    return agent;
                }
            }
        }
    }
    return 0;
}*/



/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSPhysicalEnvironment::registerAgent(GWSAgent *agent){
    /*if( !agent->property( GWSPhysicalEnvironment::GEOMETRY_PROP ) ){
        qWarning() << QString("Tried to add agent %1 %2 without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return;
    }

    this->mutex.lock();
    this->spatial_envelopes.insert( agent->getId() , agent->geometry->getEnvelope() );
    this->mutex.unlock();
    agent->mutex.unlock();

    const QMetaObject* class_type = agent->metaObject();
    QList<QString> keys = this->spatial_index.keys();
    while( class_type && !keys.contains( class_type->className() ) ){

        // Insert new quadtree with the agents class
        GWSQuadtree* index = new GWSQuadtree();
        this->spatial_index.insert( class_type->className() , index );

        class_type = class_type->superClass();
    }

    this->mutex.lock();
    class_type = agent->metaObject();
    GWSEnvelope env = this->spatial_envelopes.value( agent->getId() );
    while( class_type ){

        this->spatial_index[ class_type->className() ]->insert( env , agent );

        class_type = class_type->superClass();
    }
    this->mutex.unlock();*/

}

void GWSPhysicalEnvironment::unregisterAgent(GWSAgent *agent){

    // Retrieve its envelope
    /*this->mutex.lock();
    GWSEnvelope agent_geom_env = this->spatial_envelopes.value( agent->getId() );
    this->mutex.unlock();

    const QMetaObject* obj = agent->metaObject();
    while( obj ){

        // Remove from spatial index
        this->mutex.lock();
        GWSQuadtree* index = this->spatial_index.value( obj->className() );
        if( index ){ index->remove( agent_geom_env , agent ); }
        this->mutex.unlock();

        obj = obj->superClass();
    }

    this->mutex.lock();
    this->spatial_envelopes.remove( agent->getId() );
    this->mutex.unlock();*/

}

/*bool GWSPhysicalEnvironment::updateAgentGeometry(GWSAgent *agent, GWSCoordinate new_geom){
    return this->updateAgentGeometry( agent , GWSGeometryFactory::globalInstance()->createPoint( new_geom ) );
}*/

/*bool GWSPhysicalEnvironment::updateAgentGeometry(GWSAgent *agent, GWSGeometry *new_geom){

    // Remove old version from spatial index
    this->unregisterAgent( agent );

    // Set geometry
    agent->mutex.lock();
    if( agent->geometry ){
        agent->geometry->deleteLater();
        agent->geometry = 0;
    }
    if( new_geom ){
        agent->geometry = new_geom->createCopy();
        new_geom->deleteLater();
    }
    agent->mutex.unlock();

    // Add new version in spatial index
    this->registerAgent( agent );
    return true;
}*/


