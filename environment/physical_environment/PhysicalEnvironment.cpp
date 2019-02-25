#include "PhysicalEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../object/ObjectFactory.h"

QString GWSPhysicalEnvironment::GEOMETRY_PROP = "geometry";

GWSPhysicalEnvironment* GWSPhysicalEnvironment::globalInstance(){
    static GWSPhysicalEnvironment instance;
    return &instance;
}

GWSPhysicalEnvironment::GWSPhysicalEnvironment() : GWSEnvironment(){
    qInfo() << "PhysicalEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
    this->environment_agent_indexes.insert( GWSAgent::staticMetaObject.className() , QSharedPointer<GWSQuadtree>( new GWSQuadtree() ) );
}

GWSPhysicalEnvironment::~GWSPhysicalEnvironment(){
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
    if( this->environment_agent_indexes.keys().contains( GWSAgent::staticMetaObject.className() ) ){
        return this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent->getId() ).dynamicCast<GWSGeometry>();
    }
    return Q_NULLPTR;
}

QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    QList< QSharedPointer<GWSAgent> > agents;
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value( class_name )->getElements<GWSAgent>( minX , maxX , minY , maxY );
    }
    return agents;
}


QList< QSharedPointer<GWSAgent> > GWSPhysicalEnvironment::getAgentsIntersecting(const QSharedPointer<GWSGeometry> geometry, QString class_name) const{
    QList< QSharedPointer<GWSAgent> > agents;
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value( class_name )->getElements<GWSAgent>( geometry );
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


/**
 * @brief PhysicalEnvironment::getNearestAgents Given a list of geometries and the class_name of agents in the
 * environment, returns an ordered list of the nearest agent for each geometry.
 * @param geometry Geometry from where to start searching
 * @param class_name ClassName of the agents in the environment from which to get the nearest for geometry
 * @return
 */
QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QString class_name) const{
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value(class_name)->getNearestElement<GWSAgent>( coor );
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QList< QSharedPointer<GWSAgent> > agents ) const{
    QSharedPointer<GWSAgent> nearest = Q_NULLPTR;
    GWSLengthUnit nearest_distance = -1;
    foreach (QSharedPointer<GWSAgent> agent , agents) {
        QSharedPointer<GWSGeometry> geom = this->getGeometry( agent );
        if( geom.isNull() ){
            continue;
        }
        GWSCoordinate centroid = geom->getCentroid();
        if( nearest.isNull() ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
        if( centroid.getDistance( coor ) < nearest_distance ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
    }
    return nearest;
}

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

    if( agent.isNull() || agent->getProperty( GEOMETRY_PROP ).isNull() ){
        return;
    }

    // GEOMETRY (comes as a QJSONOBJECT, need to extract it and build a GWSGEOMETRY )
    QJsonObject geom_json = agent->getProperty( GEOMETRY_PROP ).toObject();
    QSharedPointer<GWSGeometry> geom = GWSObjectFactory::globalInstance()->fromJSON( geom_json ).dynamicCast<GWSGeometry>();

    if( geom.isNull() ){
        return;
    }

    QString agent_id = agent->getId();

    // Add the new agents geometry
    if( !this->agent_ids.contains( agent_id ) ){
        this->agent_ids.append( agent_id );
    }

    this->registerAgentToIndex( agent , geom );

    GWSEnvironment::registerAgent( agent );

    // Set geometry in agent to null, because it is be stored here in the environment
    agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , QJsonValue() );
}

void GWSPhysicalEnvironment::unregisterAgent(QSharedPointer<GWSAgent> agent){

    QString agent_id = agent->getId();
    GWSEnvironment::unregisterAgent( agent );

    foreach (QJsonValue v , agent->getInheritanceFamily()) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        if( this->environment_agent_indexes.value( family , Q_NULLPTR ) ){
            this->environment_agent_indexes.value( family )->remove( agent );
        }

    }
    this->agent_ids.removeAll( agent_id );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSPhysicalEnvironment::registerAgentToIndex(QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> geom){
    foreach (QJsonValue v , agent->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lock();
        if( !this->environment_agent_indexes.keys().contains( family ) ){
            this->environment_agent_indexes.insert( family , QSharedPointer<GWSQuadtree>( new GWSQuadtree() ) );
        }
        this->mutex.unlock();
        this->environment_agent_indexes.value( family )->upsert( agent , geom );

    }
}

/**********************************************************************
 SPATIAL OPERATIONS
**********************************************************************/

void GWSPhysicalEnvironment::transformMove(QSharedPointer<GWSAgent> agent, const GWSCoordinate &apply_movement){
    QSharedPointer<GWSGeometry> geom = this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent->getId() );
    if( geom.isNull() ){ // Create its geometry in the index
        geom = QSharedPointer< GWSGeometry >( new GWSGeometry( apply_movement ) );
        this->registerAgentToIndex( agent , geom );
    } else {
        geom->transformMove( apply_movement );
    }
}

void GWSPhysicalEnvironment::transformBuffer(QSharedPointer<GWSAgent> agent, double threshold){
    QSharedPointer<GWSGeometry> geom = this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent->getId() );
    if( geom ){
        geom->transformBuffer( threshold );
    }
}

void GWSPhysicalEnvironment::transformUnion(QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other){
    QSharedPointer<GWSGeometry> geom = this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent->getId() );if( geom ){
        geom->transformUnion( other );
    }
}

void GWSPhysicalEnvironment::transformIntersection(QSharedPointer<GWSAgent> agent, QSharedPointer<GWSGeometry> other){
    QSharedPointer<GWSGeometry> geom = this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent->getId() );
    if( geom ){
        geom->transformIntersection( other );
    }
}



