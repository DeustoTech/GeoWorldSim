#include "NetworkEnvironment.h"

#include <QTimer>

#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../object/ObjectFactory.h"

QString geoworldsim::environment::NetworkEnvironment::EDGE_PROP = "edge";

geoworldsim::environment::NetworkEnvironment* geoworldsim::environment::NetworkEnvironment::globalInstance(){
    static geoworldsim::environment::NetworkEnvironment instance;
    return &instance;
}

geoworldsim::environment::NetworkEnvironment::NetworkEnvironment() : Environment(){
    qInfo() << "NetworkEnvironment created";
    EnvironmentsGroup::globalInstance()->addEnvironment( this );
}

geoworldsim::environment::NetworkEnvironment::~NetworkEnvironment(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject geoworldsim::environment::NetworkEnvironment::serialize() const{
    QJsonObject json;
    //QJsonObject json = GWSEnvironment::serializeMini();
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::environment::NetworkEnvironment::getEdge( const geoworldsim::geometry::Coordinate &from , const geoworldsim::geometry::Coordinate &to , const QString &class_name) const{
    if( this->network_edges.keys().contains( class_name ) ){
        QString agent_id = this->network_edges.value( class_name )->getNearestElement( from );
        QSharedPointer< Entity > agent = EntityEnvironment::globalInstance()->getByUID( agent_id );
        graph::NetworkEdge edge = graph::NetworkEdge( agent->getProperty( EDGE_PROP ).toObject() );
        if( edge.getToCoordinate() == to ){
            return agent_id;
        }
    }
    return QString();
}

QString geoworldsim::environment::NetworkEnvironment::getNearestAgent( const geoworldsim::geometry::Coordinate &coor , const QString &class_name ) const{
    if( this->network_edges.keys().contains( class_name ) ){
        return this->network_edges.value( class_name )->getNearestElement( coor );
    }
    return QString();
}

QPair< geoworldsim::geometry::Coordinate , QList< QSharedPointer< geoworldsim::Entity > > > geoworldsim::environment::NetworkEnvironment::getNearestNodeAndPath(const geoworldsim::geometry::Coordinate &coor, QList< geoworldsim::geometry::Coordinate > get_nearest, const QString &class_name) const {
    QList< QStringList > routes_to_all = this->getShortestPaths( coor , get_nearest , class_name );

    Q_ASSERT( routes_to_all.size() == get_nearest.size() );

    // Check which route is shortest
    unit::LengthUnit shortest_length = unit::LengthUnit( 9999999999 );
    QPair< geoworldsim::geometry::Coordinate , QList< QSharedPointer< Entity > > > nearest_node_and_route;
    for(int i = 0 ; i < get_nearest.size() ; i++ ){

        QStringList route = routes_to_all.at( i );
        if( route.isEmpty() ){ continue; }

        QList< QSharedPointer< Entity > > route_agents = EntityEnvironment::globalInstance()->getByUIDS( route );
        if( route_agents.isEmpty() ){ continue; }

        unit::LengthUnit l = unit::LengthUnit(0);
        foreach( QSharedPointer< Entity > e , route_agents ){
            l = l + graph::NetworkEdge( e->getProperty( EDGE_PROP ).toObject() ).getLength();
        }
        if( l < shortest_length ){
            shortest_length = l;
            nearest_node_and_route = QPair< geoworldsim::geometry::Coordinate , QList< QSharedPointer< Entity > > >( get_nearest.at( i ) , route_agents );
        }
    }
    return nearest_node_and_route;
}

QStringList geoworldsim::environment::NetworkEnvironment::getShortestPath( const geoworldsim::geometry::Coordinate &from, const geoworldsim::geometry::Coordinate &to , const QString &class_name ) const{

    if( this->network_routings.keys().contains( class_name ) ){
        // Move given coordinates to real graph nodes
        QString snapped_from = this->getNearestNodeUID( from , class_name );
        QString snapped_to = this->getNearestNodeUID( to , class_name );
        return this->network_routings.value( class_name )->getShortestPath( snapped_from , snapped_to );
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QStringList();
}

QList< QStringList > geoworldsim::environment::NetworkEnvironment::getShortestPath( QList< geoworldsim::geometry::Coordinate > ordered_coors , const QString &class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        QStringList snapped_ordered;
        foreach (geoworldsim::geometry::Coordinate c, ordered_coors) {
            snapped_ordered.append( this->getNearestNodeUID( c , class_name ) );
        }
        return this->network_routings.value( class_name )->getShortestPath( snapped_ordered );
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QList< QStringList >();
}

QList< QStringList > geoworldsim::environment::NetworkEnvironment::getShortestPaths( const geoworldsim::geometry::Coordinate &from_one, QList< geoworldsim::geometry::Coordinate > to_many , const QString &class_name ) const{
    QList< QStringList > paths;

    if( this->network_routings.keys().contains( class_name ) ){

        QString snapped_from = this->getNearestNodeUID( from_one , class_name );
        if( snapped_from.isEmpty() ){
            for(int i = 0; i < to_many.size() ; i++){
                paths.append( QStringList() );
            }
            return paths;
        }

        QStringList snapped_to_many;
        foreach( geoworldsim::geometry::Coordinate c , to_many ) {
            snapped_to_many.append( this->getNearestNodeUID( c , class_name ) );
        }
        paths = this->network_routings.value( class_name )->getShortestPaths( snapped_from , snapped_to_many );
        return paths;

    }
    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return paths;
}


/**********************************************************************
 SETTERS
**********************************************************************/



/**********************************************************************
 PRIVATE
**********************************************************************/

void geoworldsim::environment::NetworkEnvironment::registerEntity( QSharedPointer< Entity > entity ){

    // If already registered
    if( entity.isNull() || entity->getEnvironments().contains( this ) || entity->getProperty( EDGE_PROP ).isNull() ){
        return;
    }

    Environment::registerEntity( entity );

    QJsonObject json = entity->getProperty( EDGE_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    graph::NetworkEdge edge = graph::NetworkEdge( json );
    if( !edge.isValid() ){
        return;
    }

    this->upsertEntityToIndex( entity , edge );

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    entity->addSubscription( EDGE_PROP , [this](QSharedPointer< Object > entity , QString property_name ){
        this->upsertEntityToIndex( entity.dynamicCast< Entity >() , graph::NetworkEdge( entity->getProperty( property_name ).toObject() ) );
    });

}

void geoworldsim::environment::NetworkEnvironment::unregisterEntity( QSharedPointer< Entity > entity ){

    try {

        Environment::unregisterEntity( entity );
        QJsonArray classes = entity->getInheritanceFamily();

        graph::NetworkEdge edge = graph::NetworkEdge( entity->getProperty( EDGE_PROP ).toObject() );
        QString uuid = entity->getUID();

        //this->disconnect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &geoworldsim::environment::NetworkEnvironment::entityPropertyChanged );

        foreach(QJsonValue v , classes){

            QString family = v.toString();
            if( family.isEmpty() ){ continue; }

            QTimer::singleShot( 10 , [this , uuid , family] {

                // Remove from spatial graph
                this->network_edges.value( family )->remove( uuid );
                this->network_routings.value( family )->removeEdge( uuid );

            });
        }

    } catch (std::exception &e){
        qWarning() << QString("Simulation threw an exception unregistering agent from geoworldsim::environment::NetworkEnvironment. Exception %1").arg( e.what() );
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void geoworldsim::environment::NetworkEnvironment::upsertEntityToIndex(QSharedPointer< Entity > entity , const graph::NetworkEdge &edge ){
    foreach( QJsonValue v , entity ->getInheritanceFamily() ) {

        QString uuid = entity ->getUID();
        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lockForRead();
        QSharedPointer< geometry::Quadtree > tree = this->network_edges.value( family , Q_NULLPTR );
        QSharedPointer< routing::Routing > routing = this->network_routings.value( family , Q_NULLPTR );

        if( !tree ){
            this->mutex.unlock();

            this->mutex.lockForWrite();
            this->environment_entity_index_types.append( family );
            tree = QSharedPointer< geometry::Quadtree >( new geometry::Quadtree() );
            tree.data()->setObjectName( QString("%1 %2").arg( this->metaObject()->className() ).arg( family ) );
            this->network_edges.insert( family , tree );
            routing = QSharedPointer< routing::Routing >( new routing::Routing() );
            this->network_routings.insert( family , routing );
        }
        this->mutex.unlock();

        emit tree->upsertCoordinateSignal( uuid , edge.getFromCoordinate() );
        emit routing->upsertEdgeSignal( uuid , edge );
    }
}

void geoworldsim::environment::NetworkEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == EDGE_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        Entity* agent = dynamic_cast< Entity* >( object );
        if( !agent ){ return; }
        this->upsertEntityToIndex( agent->getSharedPointer() , graph::NetworkEdge( agent->getProperty( EDGE_PROP ).toObject() ) );
    }
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QString geoworldsim::environment::NetworkEnvironment::getNearestNodeUID( const geoworldsim::geometry::Coordinate& coor , const QString& class_name ) const{

    if( this->network_edges.keys().contains( class_name ) ){
        QString entity_id = this->getNearestAgent( coor , class_name );
        if( entity_id.isEmpty() ){
            return entity_id;
        }

        QSharedPointer< Entity> agent = EntityEnvironment::globalInstance()->getByUID( entity_id );
        graph::NetworkEdge edge = graph::NetworkEdge( agent->getProperty( EDGE_PROP ).toObject() );

        if( !edge.isValid() ){
            return QString();
        }

        unit::LengthUnit from_distance = coor.getDistance( edge.getFromCoordinate() );
        unit::LengthUnit to_distance = coor.getDistance( edge.getToCoordinate() );
        return from_distance < to_distance ? edge.getFromNodeUID() : edge.getToNodeUID();
    }

    return QString();
}

