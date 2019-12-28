#include "PhysicalEnvironment.h"

#include <QTimer>

#include "../../environment/EnvironmentsGroup.h"
#include "../../object/ObjectFactory.h"
#include "../../util/geometry/GeometryTransformators.h"

QString geoworldsim::environment::PhysicalEnvironment::GEOMETRY_PROP = "geometry";

geoworldsim::environment::PhysicalEnvironment* geoworldsim::environment::PhysicalEnvironment::globalInstance(){
    static geoworldsim::environment::PhysicalEnvironment instance;
    return &instance;
}

geoworldsim::environment::PhysicalEnvironment::PhysicalEnvironment() : Environment(){
    qInfo() << "PhysicalEnvironment created";
    EnvironmentsGroup::globalInstance()->addEnvironment( this );
    this->environment_entity_indexes.insert( Entity::staticMetaObject.className() , QSharedPointer< geometry::Quadtree >( new geometry::Quadtree() ) );
}

geoworldsim::environment::PhysicalEnvironment::~PhysicalEnvironment(){
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

const geoworldsim::geometry::Geometry geoworldsim::environment::PhysicalEnvironment::getBounds() const{
    geometry::Geometry bounds;
    foreach( QSharedPointer<geometry::Quadtree> t , this->environment_entity_indexes.values() ) {
        bounds = geometry::GeometryTransformators::transformToFit( bounds , t->getBounds() );
    }
    return bounds;
}

const geoworldsim::geometry::Geometry geoworldsim::environment::PhysicalEnvironment::getBounds(QString &class_name ) const {
    QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( class_name.toStdString() , Q_NULLPTR );
    if( tree ){
        return tree->getBounds();
    }
    return geometry::Geometry();
}

geoworldsim::geometry::Coordinate geoworldsim::environment::PhysicalEnvironment::getRandomCoordinate() const{
    return this->getBounds().getCentroid();
}

const geoworldsim::geometry::Geometry geoworldsim::environment::PhysicalEnvironment::getGeometry( QSharedPointer< Entity > agent ) const{
    return this->getGeometry( agent->getUID() );
}

const geoworldsim::geometry::Geometry geoworldsim::environment::PhysicalEnvironment::getGeometry(const QString &agent_id ) const{
    if( this->environment_entity_indexes.keys().contains( Entity::staticMetaObject.className() ) ){
        return this->environment_entity_indexes.value( Entity::staticMetaObject.className() )->getGeometry( agent_id );
    }
    return geometry::Geometry();
}

QStringList geoworldsim::environment::PhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    QStringList agents;
    QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( class_name.toStdString() , Q_NULLPTR );
    if( tree ){
        return tree->getElements( minX , maxX , minY , maxY );
    }
    return agents;
}


QStringList geoworldsim::environment::PhysicalEnvironment::getAgentsIntersecting( const geoworldsim::geometry::Geometry &geometry , QString class_name) const{
    QStringList agents;
    QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( class_name.toStdString() , Q_NULLPTR );
    if( tree ){
        return tree->getElements( geometry );
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
QStringList geoworldsim::environment::PhysicalEnvironment::getNearestAgents(QList< geoworldsim::geometry::Coordinate > coors, QString class_name) const{
    QStringList founds;
    foreach( geometry::Coordinate coor , coors ){
        founds.append( this->getNearestEntity( coor , class_name ) );
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
QString geoworldsim::environment::PhysicalEnvironment::getNearestEntity(const geoworldsim::geometry::Coordinate &coor, QString class_name) const{
    QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( class_name.toStdString() , Q_NULLPTR );
    if( tree ){
        return tree->getNearestElement( coor );
    }
    return QString();
}

QSharedPointer< geoworldsim::Entity> geoworldsim::environment::PhysicalEnvironment::getNearestEntity(const geoworldsim::geometry::Coordinate &coor, QList< QSharedPointer< geoworldsim::Entity > > agents ) const{
    QSharedPointer< Entity> nearest = Q_NULLPTR;
    unit::LengthUnit nearest_distance = -1;
    foreach (QSharedPointer< Entity > agent , agents) {
        geometry::Geometry geom = geometry::Geometry( agent->getProperty( GEOMETRY_PROP ).toObject() );
        if( !geom.isValid() ){
            continue;
        }
        const geometry::Coordinate& centroid = geom.getCentroid();
        if( nearest_distance < unit::LengthUnit( 0 ) || nearest.isNull() ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
        if( nearest_distance < unit::LengthUnit( 0 ) || centroid.getDistance( coor ) < nearest_distance ){
            nearest = agent;
            nearest_distance = centroid.getDistance( coor );
        }
    }
    return nearest;
}

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::environment::PhysicalEnvironment::registerEntity(QSharedPointer< Entity > agent ){

    if( agent.isNull() || agent->getProperty( GEOMETRY_PROP ).isNull() ){
        return;
    }

    Environment::registerEntity( agent );

    QJsonObject json = agent->getProperty( GEOMETRY_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    geometry::Geometry geom = geometry::Geometry( json );
    if( !geom.isValid() ){
        return;
    }

    this->upsertEntityToIndex( agent , geom );

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    agent->addSubscription( GEOMETRY_PROP , [this](QSharedPointer< Object > entity , QString property_name ){
        this->upsertEntityToIndex( entity.dynamicCast< Entity >() , geometry::Geometry( entity->getProperty( property_name ).toObject() ) );
    });

}

void geoworldsim::environment::PhysicalEnvironment::unregisterEntity(QSharedPointer< Entity > entity){

    Environment::unregisterEntity( entity );

    //this->disconnect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &geoworldsim::environment::PhysicalEnvironment::entityPropertyChanged );

    QString uuid = entity->getUID();

    foreach (QJsonValue v , entity->getInheritanceFamily()) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( family.toStdString() , Q_NULLPTR );
        if( tree ){
            emit tree->removeSignal( uuid );
        }
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void geoworldsim::environment::PhysicalEnvironment::upsertEntityToIndex(QSharedPointer< Entity > entity, const geoworldsim::geometry::Geometry &geom){

    if( !entity ){ return; }

    QString uuid = entity->getUID();

    foreach (QJsonValue v , entity->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        QSharedPointer< geometry::Quadtree > tree = this->environment_entity_indexes.value( family.toStdString() , Q_NULLPTR );
        if( !tree ){
            this->environment_entity_index_types.append( family );
            tree = QSharedPointer< geometry::Quadtree >( new geometry::Quadtree() );
            tree.data()->setObjectName( QString("%1 %2").arg( this->metaObject()->className() ).arg( family ) );
            this->environment_entity_indexes.insert( family.toStdString() , tree );
        }

        emit tree->upsertGeometrySignal( uuid , geom );
    }
}

void geoworldsim::environment::PhysicalEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == GEOMETRY_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        Entity* agent = dynamic_cast<Entity*>( object );
        if( !agent ){ return; }
        this->upsertEntityToIndex( agent->getSharedPointer() , agent->getProperty( GEOMETRY_PROP ).toObject() );
    }
}

