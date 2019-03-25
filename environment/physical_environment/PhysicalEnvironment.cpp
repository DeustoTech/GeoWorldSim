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

const GWSGeometry GWSPhysicalEnvironment::getBounds() const{
    return this->environment_bounds;
}

GWSCoordinate GWSPhysicalEnvironment::getRandomCoordinate() const{
    return this->environment_bounds.getCentroid();
}

const GWSGeometry GWSPhysicalEnvironment::getGeometry( QSharedPointer<GWSAgent> agent ) const{
    return this->getGeometry( agent->getUID() );
}

const GWSGeometry GWSPhysicalEnvironment::getGeometry( QString agent_id ) const{
    if( this->environment_agent_indexes.keys().contains( GWSAgent::staticMetaObject.className() ) ){
        return this->environment_agent_indexes.value( GWSAgent::staticMetaObject.className() )->getGeometry( agent_id );
    }
    return GWSGeometry();
}

QStringList GWSPhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    QStringList agents;
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value( class_name )->getElements( minX , maxX , minY , maxY );
    }
    return agents;
}


QStringList GWSPhysicalEnvironment::getAgentsIntersecting( const GWSGeometry geometry , QString class_name) const{
    QStringList agents;
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value( class_name )->getElements( geometry );
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
QStringList GWSPhysicalEnvironment::getNearestAgents(QList<GWSCoordinate> coors, QString class_name) const{
    QStringList founds;
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
QString GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QString class_name) const{
    if( this->environment_agent_indexes.keys().contains(class_name) ){
        return this->environment_agent_indexes.value(class_name)->getNearestElement( coor );
    }
    return QString();
}

QSharedPointer<GWSAgent> GWSPhysicalEnvironment::getNearestAgent(GWSCoordinate coor, QList< QSharedPointer<GWSAgent> > agents ) const{
    QSharedPointer<GWSAgent> nearest = Q_NULLPTR;
    GWSLengthUnit nearest_distance = -1;
    foreach (QSharedPointer<GWSAgent> agent , agents) {
        GWSGeometry geom = this->getGeometry( agent );
        if( !geom.isValid() ){
            continue;
        }
        GWSCoordinate centroid = geom.getCentroid();
        if( nearest_distance < GWSLengthUnit( 0 ) || nearest.isNull() ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
        if( nearest_distance < GWSLengthUnit( 0 ) || centroid.getDistance( coor ) < nearest_distance ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
    }
    return nearest;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSPhysicalEnvironment::setBounds(GWSGeometry geom){
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
    GWSGeometry geom = GWSGeometry( geom_json );

    // Listen to agent property changes
    this->connect( agent.data() , &GWSAgent::propertyChangedSignal , this , &GWSPhysicalEnvironment::agentPropertyChanged );

    if( !geom.isValid() ){
        return;
    }

    QString agent_uid = agent->getUID();

    if ( agent_uid.isEmpty() ){
        return;
    }

    // Add the new agents geometry
    if( !this->agent_ids.contains( agent_uid ) ){
        this->agent_ids.append( agent_uid );
    }

    this->registerAgentToIndex( agent , geom );

    GWSEnvironment::registerAgent( agent );

}

void GWSPhysicalEnvironment::unregisterAgent(QSharedPointer<GWSAgent> agent){

    QString agent_id = agent->getUID();
    GWSEnvironment::unregisterAgent( agent );

    this->disconnect( agent.data() , &GWSAgent::propertyChangedSignal , this , &GWSPhysicalEnvironment::agentPropertyChanged );

    foreach (QJsonValue v , agent->getInheritanceFamily()) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        if( this->environment_agent_indexes.value( family , Q_NULLPTR ) ){
            this->environment_agent_indexes.value( family )->remove( agent_id );
        }
    }
    this->agent_ids.removeAll( agent_id );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSPhysicalEnvironment::registerAgentToIndex(QSharedPointer<GWSAgent> agent, GWSGeometry geom){
    foreach (QJsonValue v , agent->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lock();
        if( !this->environment_agent_indexes.keys().contains( family ) ){
            this->environment_agent_indexes.insert( family , QSharedPointer<GWSQuadtree>( new GWSQuadtree() ) );
        }
        this->mutex.unlock();
        this->environment_agent_indexes.value( family )->upsert( agent->getUID() , geom );

    }
}

void GWSPhysicalEnvironment::agentPropertyChanged( QString property_name ){
    if( property_name == GEOMETRY_PROP ){
        QObject* object = QObject::sender();
        GWSAgent* agent = dynamic_cast<GWSAgent*>( object );
        qDebug() << property_name;
    }
}

