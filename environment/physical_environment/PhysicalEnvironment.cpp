
#include "PhysicalEnvironment.h"

QString GWSPhysicalEnvironment::GEOMETRY_PROP = "geo";

GWSPhysicalEnvironment* GWSPhysicalEnvironment::globalInstance(){
    static GWSPhysicalEnvironment instance;
    return &instance;
}

GWSPhysicalEnvironment::GWSPhysicalEnvironment() : GWSEnvironment(){
    qInfo() << "PhysicalEnvironment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSPhysicalEnvironment::~GWSPhysicalEnvironment(){
    qDeleteAll( this->spatial_index.values() );
    qDeleteAll( this->agent_geometries.values() );
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

const GWSGeometry* GWSPhysicalEnvironment::getGeometry(QString agent_id) const{
    return this->agent_geometries.value( agent_id , 0 );
}

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


QList<GWSAgent*> GWSPhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    if( this->spatial_index.keys().contains(class_name) ){
        return this->spatial_index[class_name]->getElements( minX , maxX , minY , maxY );
    }
    return QList<GWSAgent*>();
}


QList<GWSAgent*> GWSPhysicalEnvironment::getAgentsIntersecting(const GWSGeometry* geometry, QString class_name) const{
    if( this->spatial_index.keys().contains( class_name ) ){
        return this->spatial_index[class_name]->getElements( geometry );
    }
    return QList<GWSAgent*>();
}

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
 METHODS
**********************************************************************/

void GWSPhysicalEnvironment::registerAgent(GWSAgent *agent , GWSGeometry* init_geom ){

    QString agent_id = agent->getId();

    // Remove if existing
    if( this->agent_ids.contains( agent_id ) ){
        delete this->agent_geometries.value( agent_id );
    }

    // Add the new agents geometry
    this->agent_ids.append( agent_id );
    this->agent_geometries.insert( agent_id , init_geom ? init_geom : new GWSGeometry() );

    foreach (QString s , agent->getInheritanceFamily()) {
        if( !this->spatial_index.keys().contains(s) ){
            this->spatial_index.insert( s , new GWSQuadtree() );
        }
    }

    this->mutex.lock();
    GWSEnvironment::registerAgent( agent );
    foreach (QString s , agent->getInheritanceFamily()) {
        this->spatial_index.value( s )->upsert( agent );
    }
    this->mutex.unlock();

}

void GWSPhysicalEnvironment::unregisterAgent(GWSAgent *agent){

    this->mutex.lock();
    QString agent_id = agent->getId();
    GWSEnvironment::unregisterAgent( agent );
    foreach (QString s , agent->getInheritanceFamily()) {
        this->spatial_index.value( s )->remove( agent );
    }
    this->mutex.unlock();

    //delete this->agent_geometries.value( agent->getId() );
    this->agent_geometries.remove( agent_id );
    this->agent_ids.removeAll( agent_id );

}


/**********************************************************************
 SPATIAL OPERATIONS
**********************************************************************/

void GWSPhysicalEnvironment::transformMove(GWSAgent *agent, const GWSCoordinate &apply_movement){
    GWSGeometry* geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        foreach (QString s , agent->getInheritanceFamily()) {
            this->spatial_index.value( s )->upsert( agent );
        }
        geom->transformMove( apply_movement );
    }
}

void GWSPhysicalEnvironment::transformBuffer(GWSAgent *agent, double threshold){
    GWSGeometry* geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformBuffer( threshold );
    }
}

void GWSPhysicalEnvironment::transformUnion(GWSAgent *agent, const GWSGeometry *other){
    GWSGeometry* geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformUnion( other );
    }
}

void GWSPhysicalEnvironment::transformIntersection(GWSAgent *agent, const GWSGeometry *other){
    GWSGeometry* geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformIntersection( other );
    }
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


