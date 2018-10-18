#include "PhysicalEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

QString GWSPhysicalEnvironment::GEOMETRY_PROP = "geo";

GWSPhysicalEnvironment* GWSPhysicalEnvironment::globalInstance(){
    static GWSPhysicalEnvironment instance;
    return &instance;
}

GWSPhysicalEnvironment::GWSPhysicalEnvironment() : GWSEnvironment(){
    qInfo() << "PhysicalEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSPhysicalEnvironment::~GWSPhysicalEnvironment(){
    qDeleteAll( this->spatial_index.values() );
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

QSharedPointer<GWSGeometry> GWSPhysicalEnvironment::getBounds() const{
    return this->environment_bounds;
}

GWSCoordinate GWSPhysicalEnvironment::getRandomCoordinate() const{
    if( this->environment_bounds.isNull() ){
        return GWSCoordinate( 0 , 0 , 0 );
    }
    return this->environment_bounds->getCentroid();
}

QSharedPointer<GWSGeometry> GWSPhysicalEnvironment::getGeometry( QSharedPointer<GWSAgent> agent ) const{
    return this->agent_geometries.value( agent->getId() , Q_NULLPTR );
}

QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::orderByDistance(QSharedPointer<GWSAgent> source, QList<QSharedPointer<GWSAgent>> agents) const{
    QList<QSharedPointer<GWSAgent>> ordered;
    /*if( !agents.size() ){
        return ordered;
    }

     while( agents.size() ){
        QSharedPointer<GWSAgent> nearest = agents.at( 0 );
        const QSharedPointer<GWSGeometry> nearest_geom = nearest->getGeometry();

        foreach( QSharedPointer<GWSAgent> agent , agents ){
            const QSharedPointer<GWSGeometry> agent_geom = agent->getGeometry();

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


QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    QList< QSharedPointer<GWSAgent> > agents;
    if( this->spatial_index.keys().contains(class_name) ){
        foreach( QSharedPointer<GWSAgent> a , this->spatial_index.value( class_name )->getElements<GWSAgent>( minX , maxX , minY , maxY ) ){
            agents.append( a );
        }
    }
    return agents;
}


QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getAgentsIntersecting(const QSharedPointer<GWSGeometry> geometry, QString class_name) const{
    QList< QSharedPointer<GWSAgent> > agents;
    if( this->spatial_index.keys().contains( class_name ) ){
        foreach( QSharedPointer<GWSObject> obj , this->spatial_index.value( class_name )->getElements<GWSAgent>( geometry ) ){
            agents.append( obj.dynamicCast<GWSAgent>() );
        }
    }
    return agents;
}

/**
 * @brief PhysicalEnvironment::getNearestAgents Given a list of geometries and the class_name of agents in the
 * environment, returns an ordered list of the nearest agent for each geometry.
 * IMPORTANT, if an agent is not snapped it will return a 0 agent to preserve list size and order.
 * @param geometries List of geometries from where to start searching
 * @param class_name ClassName of the agents in the environment from which to get the nearest for each geometry
 * @return
 */
QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getNearestAgents(QList<GWSCoordinate> coors, QString class_name) const{
    QList<QSharedPointer<GWSAgent>> founds = QList<QSharedPointer<GWSAgent>>();
    foreach(GWSCoordinate coor , coors){
        founds.append( this->getNearestAgent( coor , class_name ) );
    }
    return founds;
}

QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QList<QSharedPointer<GWSAgent> > agents) const{
    QList<GWSCoordinate> coors;
    coors.append( coor );
    QList<QSharedPointer<GWSAgent>> nearests = this->getNearestAgents( coors , agents );
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
QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QString class_name) const{
    if( this->spatial_index.keys().contains(class_name) ){
        return this->spatial_index.value(class_name)->getNearestElement( coor ).dynamicCast<GWSAgent>();
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
QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getNearestAgents(QList<GWSCoordinate> coors, QList<QSharedPointer<GWSAgent>> agents) const{
    QList< QSharedPointer<GWSAgent> > founds;

    GWSQuadtree* index = new GWSQuadtree();
    foreach(QSharedPointer<GWSAgent> a , agents){
        index->upsert( a , this->getGeometry( a ) );
    }

    foreach(GWSCoordinate coor , coors){
        QSharedPointer<GWSAgent> found = 0;
        QList< QSharedPointer<GWSAgent> > agents = index->getElements<GWSAgent>( coor );

        if( !agents.isEmpty() ){
            found = agents.at( qrand() % agents.size() );
            foreach( QSharedPointer<GWSAgent> a , agents ){
                if( a && coor.getDistance( this->getGeometry( a )->getCentroid() ) < coor.getDistance( this->getGeometry( found )->getCentroid() ) ){
                    found = a;
                }
            }
        }
        founds.append( found );
    }

    index->deleteLater();
    return founds;
}


/*QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getAgentByGeometry(QSharedPointer<GWSGeometry> geometry, QString class_name) const{
    if( this->spatial_index.contains(class_name) ){

        QList<void *> objs = this->spatial_index[class_name]->getElements( geometry->getEnvelope() );

        if( !objs.isEmpty() ){
            foreach(void * o , objs){
                QSharedPointer<GWSAgent> agent = ( (QSharedPointer<GWSAgent>) o);
                if( agent->getGeometry()->equals( geometry ) ){
                    return agent;
                }
            }
        }
    }
    return 0;
}*/


/**********************************************************************
 SETTERS
**********************************************************************/

void GWSPhysicalEnvironment::setBounds(QSharedPointer<GWSGeometry> geom){
    this->environment_bounds = geom;
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSPhysicalEnvironment::registerAgent(QSharedPointer<GWSAgent> agent ){

    if( agent.isNull() || agent->getEnvironments().contains( this ) ){
        return;
    }

    // GEOMETRY (comes parsed by GWSObject, extract and set it to null)
    QSharedPointer<GWSGeometry> geom = agent->getProperty( GEOMETRY_PROP ).value< QSharedPointer<GWSObject> >().dynamicCast<GWSGeometry>();
    if( geom.isNull() ){
        geom = QSharedPointer<GWSGeometry>( new GWSGeometry( GWSCoordinate( 0 , 0 , 0 ) ) );
    }

    QString agent_id = agent->getId();

    // Add the new agents geometry
    if( !this->agent_ids.contains( agent_id ) ){
        this->agent_ids.append( agent_id );
    }
    this->agent_geometries.insert( agent_id , geom );

    foreach( QString s , agent->getInheritanceFamily() ) {
        if( !this->spatial_index.keys().contains(s) ){
            this->spatial_index.insert( s , new GWSQuadtree() );
        }
    }

    GWSEnvironment::registerAgent( agent );

    QSharedPointer<GWSGeometry> agent_geom = this->getGeometry( agent );
    foreach (QString s , agent->getInheritanceFamily()) {
        this->spatial_index.value( s )->upsert( agent , agent_geom );
    }

    // Set geometry in agent to null, because it is be stored here in the environment
    agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , QVariant() );
}

void GWSPhysicalEnvironment::unregisterAgent(QSharedPointer<GWSAgent> agent){

    this->mutex.lock();
    QString agent_id = agent->getId();
    GWSEnvironment::unregisterAgent( agent );
    foreach( QString s , agent->getInheritanceFamily() ) {
        if( this->spatial_index.keys().contains( s ) ){
            this->spatial_index.value( s )->remove( agent );
        }
    }
    this->mutex.unlock();

    this->agent_geometries[ agent_id ].clear();
    this->agent_geometries.remove( agent_id );
    this->agent_ids.removeAll( agent_id );

}


/**********************************************************************
 SPATIAL OPERATIONS
**********************************************************************/

void GWSPhysicalEnvironment::transformMove(QSharedPointer<GWSAgent> agent, const GWSCoordinate &apply_movement){
    QSharedPointer<GWSGeometry> geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformMove( apply_movement );
        foreach (QString s , agent->getInheritanceFamily()) {
            this->spatial_index.value( s )->upsert( agent , this->getGeometry( agent ) );
        }
    }
}

void GWSPhysicalEnvironment::transformBuffer(QSharedPointer<GWSAgent> agent, double threshold){
    QSharedPointer<GWSGeometry> geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformBuffer( threshold );
    }
}

void GWSPhysicalEnvironment::transformUnion(QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other){
    QSharedPointer<GWSGeometry> geom = this->agent_geometries.value( agent->getId() );
    if( geom ){
        geom->transformUnion( other );
    }
}

void GWSPhysicalEnvironment::transformIntersection(QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other){
    QSharedPointer<GWSGeometry> geom = this->agent_geometries.value( agent->getId() );
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


