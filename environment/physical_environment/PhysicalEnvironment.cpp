#include "PhysicalEnvironment.h"

#include <QTimer>

#include "../../environment/EnvironmentsGroup.h"
#include "../../object/ObjectFactory.h"
#include "../../util/geometry/GeometryTransformators.h"

QString GWSPhysicalEnvironment::GEOMETRY_PROP = "geometry";

GWSPhysicalEnvironment* GWSPhysicalEnvironment::globalInstance(){
    static GWSPhysicalEnvironment instance;
    return &instance;
}

GWSPhysicalEnvironment::GWSPhysicalEnvironment() : GWSEnvironment(){
    qInfo() << "PhysicalEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
    this->environment_entity_indexes.insert( GWSEntity::staticMetaObject.className() , QSharedPointer<GWSQuadtree>( new GWSQuadtree() ) );
}

GWSPhysicalEnvironment::~GWSPhysicalEnvironment(){
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

const GWSGeometry GWSPhysicalEnvironment::getBounds() const{
    GWSGeometry bounds;
    foreach( QSharedPointer<GWSQuadtree> t , this->environment_entity_indexes.values() ) {
        bounds = GWSGeometryTransformators::transformToFit( bounds , t->getBounds() );
    }
    return bounds;
}

const GWSGeometry GWSPhysicalEnvironment::getBounds(QString &class_name ) const {
    if( this->environment_entity_indexes.keys().contains( class_name ) ){
        return this->environment_entity_indexes.value( class_name )->getBounds();
    }
    return GWSGeometry();
}

GWSCoordinate GWSPhysicalEnvironment::getRandomCoordinate() const{
    return this->getBounds().getCentroid();
}

const GWSGeometry GWSPhysicalEnvironment::getGeometry( QSharedPointer<GWSEntity> agent ) const{
    return this->getGeometry( agent->getUID() );
}

const GWSGeometry GWSPhysicalEnvironment::getGeometry(const QString &agent_id ) const{
    if( this->environment_entity_indexes.keys().contains( GWSEntity::staticMetaObject.className() ) ){
        return this->environment_entity_indexes.value( GWSEntity::staticMetaObject.className() )->getGeometry( agent_id );
    }
    return GWSGeometry();
}

QStringList GWSPhysicalEnvironment::getAgentsInsideBounds(double minX, double maxX, double minY, double maxY, QString class_name) const{
    QStringList agents;
    if( this->environment_entity_indexes.keys().contains(class_name) ){
        return this->environment_entity_indexes.value( class_name )->getElements( minX , maxX , minY , maxY );
    }
    return agents;
}


QStringList GWSPhysicalEnvironment::getAgentsIntersecting( const GWSGeometry &geometry , QString class_name) const{
    QStringList agents;
    if( this->environment_entity_indexes.keys().contains(class_name) ){
        return this->environment_entity_indexes.value( class_name )->getElements( geometry );
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
QString GWSPhysicalEnvironment::getNearestAgent(const GWSCoordinate &coor, QString class_name) const{
    if( this->environment_entity_indexes.keys().contains(class_name) ){
        return this->environment_entity_indexes.value(class_name)->getNearestElement( coor );
    }
    return QString();
}

QSharedPointer<GWSEntity> GWSPhysicalEnvironment::getNearestAgent(const GWSCoordinate &coor, QList< QSharedPointer<GWSEntity> > agents ) const{
    QSharedPointer<GWSEntity> nearest = Q_NULLPTR;
    GWSLengthUnit nearest_distance = -1;
    foreach (QSharedPointer<GWSEntity> agent , agents) {
        GWSGeometry geom = GWSGeometry( agent->getProperty( GEOMETRY_PROP ).toObject() );
        if( !geom.isValid() ){
            continue;
        }
        const GWSCoordinate& centroid = geom.getCentroid();
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
 METHODS
**********************************************************************/

void GWSPhysicalEnvironment::registerEntity(QSharedPointer<GWSEntity> agent ){

    if( agent.isNull() || agent->getProperty( GEOMETRY_PROP ).isNull() ){
        return;
    }

    GWSEnvironment::registerEntity( agent );

    QJsonObject json = agent->getProperty( GEOMETRY_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    GWSGeometry geom = GWSGeometry( json );
    if( !geom.isValid() ){
        return;
    }

    this->upsertEntityToIndex( agent , geom );

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    agent->addSubscription( GEOMETRY_PROP , [this](QSharedPointer<GWSObject> entity , QString property_name ){
        this->upsertEntityToIndex( entity.dynamicCast<GWSEntity>() , GWSGeometry( entity->getProperty( property_name ).toObject() ) );
    });

}

void GWSPhysicalEnvironment::unregisterEntity(QSharedPointer<GWSEntity> agent){

    QString agent_id = agent->getUID();
    GWSEnvironment::unregisterEntity( agent );

    //this->disconnect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &GWSPhysicalEnvironment::entityPropertyChanged );

    QString uuid = agent->getUID();

    foreach (QJsonValue v , agent->getInheritanceFamily()) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        if( this->environment_entity_indexes.value( family , Q_NULLPTR ) ){

            QTimer::singleShot( 10 , [this , uuid , family] {
                this->environment_entity_indexes.value( family )->remove( uuid );
            });

        }
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSPhysicalEnvironment::upsertEntityToIndex(QSharedPointer<GWSEntity> agent, const GWSGeometry &geom){

    if( !agent ){ return; }

    QString uuid = agent->getUID();

    foreach (QJsonValue v , agent->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        QSharedPointer<GWSQuadtree> tree = this->environment_entity_indexes.value( family , Q_NULLPTR );
        if( !tree ){
            this->environment_entity_index_types.append( family );
            tree = QSharedPointer<GWSQuadtree>( new GWSQuadtree() );
            this->environment_entity_indexes.insert( family , tree );
        }

        emit tree->upsertGeometry( uuid , geom );
    }
}

void GWSPhysicalEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == GEOMETRY_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        GWSEntity* agent = dynamic_cast<GWSEntity*>( object );
        if( !agent ){ return; }
        this->upsertEntityToIndex( agent->getSharedPointer() , agent->getProperty( GEOMETRY_PROP ).toObject() );
    }
}

