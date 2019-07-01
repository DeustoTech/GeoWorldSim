#include "NetworkEnvironment.h"

#include <QTimer>

#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../object/ObjectFactory.h"

#include "../../app/App.h"

QString GWSNetworkEnvironment::EDGE_PROP = "edge";

GWSNetworkEnvironment* GWSNetworkEnvironment::globalInstance(){
    static GWSNetworkEnvironment instance;
    return &instance;
}

GWSNetworkEnvironment::GWSNetworkEnvironment() : GWSEnvironment(){
    qInfo() << "NetworkEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSNetworkEnvironment::~GWSNetworkEnvironment(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSNetworkEnvironment::serialize() const{
    QJsonObject json;
    //QJsonObject json = GWSEnvironment::serializeMini();
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNetworkEnvironment::getEdge( const GWSCoordinate &from , const GWSCoordinate &to , const QString &class_name) const{
    if( this->network_edges.keys().contains( class_name ) ){
        QString agent_id = this->network_edges.value( class_name )->getNearestElement( from );
        QSharedPointer< GWSEntity > agent = GWSEntityEnvironment::globalInstance()->getByUID( agent_id );
        GWSNetworkEdge edge = GWSNetworkEdge( agent->getProperty( EDGE_PROP ).toObject() );
        if( edge.getToCoordinate() == to ){
            return agent_id;
        }
    }
    return QString();
}

QString GWSNetworkEnvironment::getNearestAgent( const GWSCoordinate &coor , const QString &class_name ) const{
    if( this->network_edges.keys().contains( class_name ) ){
        return this->network_edges.value( class_name )->getNearestElement( coor );
    }
    return QString();
}

QPair< GWSCoordinate , QList< QSharedPointer<GWSEntity> > > GWSNetworkEnvironment::getNearestNodeAndPath(const GWSCoordinate &coor, QList<GWSCoordinate> get_nearest, const QString &class_name) const {
    QList< QStringList > routes_to_all = this->getShortestPaths( coor , get_nearest , class_name );

    Q_ASSERT( routes_to_all.size() == get_nearest.size() );

    // Check which route is shortest
    GWSLengthUnit shortest_length = GWSLengthUnit( 9999999999 );
    QPair< GWSCoordinate , QList< QSharedPointer<GWSEntity> > > nearest_node_and_route;
    for(int i = 0 ; i < get_nearest.size() ; i++ ){

        QStringList route = routes_to_all.at( i );
        if( route.isEmpty() ){ continue; }

        QList< QSharedPointer<GWSEntity> > route_agents = GWSEntityEnvironment::globalInstance()->getByUIDS( route );
        if( route_agents.isEmpty() ){ continue; }

        GWSLengthUnit l = GWSLengthUnit(0);
        foreach( QSharedPointer<GWSEntity> e , route_agents ){
            l = l + GWSNetworkEdge( e->getProperty( EDGE_PROP ).toObject() ).getLength();
        }
        if( l < shortest_length ){
            shortest_length = l;
            nearest_node_and_route = QPair< GWSCoordinate , QList< QSharedPointer<GWSEntity> > >( get_nearest.at( i ) , route_agents );
        }
    }
    return nearest_node_and_route;
}

QStringList GWSNetworkEnvironment::getShortestPath( const GWSCoordinate &from, const GWSCoordinate &to , const QString &class_name ) const{

    if( this->network_routings.keys().contains( class_name ) ){
        // Move given coordinates to real graph nodes
        QString snapped_from = this->getNearestNodeUID( from , class_name );
        QString snapped_to = this->getNearestNodeUID( to , class_name );
        return this->network_routings.value( class_name )->getShortestPath( snapped_from , snapped_to );
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QStringList();
}

QList< QStringList > GWSNetworkEnvironment::getShortestPath( QList< GWSCoordinate > ordered_coors , const QString &class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        QStringList snapped_ordered;
        foreach (GWSCoordinate c, ordered_coors) {
            snapped_ordered.append( this->getNearestNodeUID( c , class_name ) );
        }
        return this->network_routings.value( class_name )->getShortestPath( snapped_ordered );
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QList< QStringList >();
}

QList< QStringList > GWSNetworkEnvironment::getShortestPaths( const GWSCoordinate &from_one, QList< GWSCoordinate > to_many , const QString &class_name ) const{
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
        foreach( GWSCoordinate c , to_many ) {
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

void GWSNetworkEnvironment::registerEntity( QSharedPointer<GWSEntity> agent ){

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) || agent->getProperty( EDGE_PROP ).isNull() ){
        return;
    }

    GWSEnvironment::registerEntity( agent );

    QJsonObject json = agent->getProperty( EDGE_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    GWSNetworkEdge edge = GWSNetworkEdge( json );
    if( !edge.isValid() ){
        return;
    }

    this->upsertEntityToIndex( agent , edge );

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    agent->addSubscription( EDGE_PROP , [this](QSharedPointer<GWSObject> entity , QString property_name ){
        this->upsertEntityToIndex( entity.dynamicCast<GWSEntity>() , GWSNetworkEdge( entity->getProperty( property_name ).toObject() ) );
    });

}

void GWSNetworkEnvironment::unregisterEntity( QSharedPointer<GWSEntity> agent ){

    try {

        GWSEnvironment::unregisterEntity( agent );
        QJsonArray classes = agent->getInheritanceFamily();

        GWSNetworkEdge edge = GWSNetworkEdge( agent->getProperty( EDGE_PROP ).toObject() );
        QString uuid = agent->getUID();

        //this->disconnect( agent.data() , &GWSEntity::entityPropertyChangedSignal , this , &GWSNetworkEnvironment::entityPropertyChanged );

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
        qWarning() << QString("Simulation threw an exception unregistering agent from GWSNetworkEnvironment. Exception %1").arg( e.what() );
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSNetworkEnvironment::upsertEntityToIndex(QSharedPointer<GWSEntity> agent, const GWSNetworkEdge &edge ){
    foreach( QJsonValue v , agent->getInheritanceFamily() ) {

        QString uuid = agent->getUID();
        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lockForRead();
        QSharedPointer< GWSQuadtree > tree = this->network_edges.value( family , Q_NULLPTR );
        QSharedPointer< GWSRouting > routing = this->network_routings.value( family , Q_NULLPTR );

        if( !tree ){
            this->mutex.unlock();

            this->mutex.lockForWrite();
            this->environment_entity_index_types.append( family );
            tree = QSharedPointer< GWSQuadtree >( new GWSQuadtree() );
            tree.data()->setObjectName( QString("%1 %2").arg( this->metaObject()->className() ).arg( family ) );
            this->network_edges.insert( family , tree );
            routing = QSharedPointer< GWSRouting >( new GWSRouting() );
            this->network_routings.insert( family , routing );
        }
        this->mutex.unlock();

        emit tree->upsertCoordinateSignal( uuid , edge.getFromCoordinate() );
        emit routing->upsertEdgeSignal( uuid , edge );
    }
}

void GWSNetworkEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == EDGE_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        GWSEntity* agent = dynamic_cast<GWSEntity*>( object );
        if( !agent ){ return; }
        this->upsertEntityToIndex( agent->getSharedPointer() , GWSNetworkEdge( agent->getProperty( EDGE_PROP ).toObject() ) );
    }
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QString GWSNetworkEnvironment::getNearestNodeUID( const GWSCoordinate& coor , const QString& class_name ) const{

    if( this->network_edges.keys().contains( class_name ) ){
        QString agent_id = this->getNearestAgent( coor , class_name );
        if( agent_id.isEmpty() ){
            return agent_id;
        }

        QSharedPointer<GWSEntity> agent = GWSEntityEnvironment::globalInstance()->getByUID( agent_id );
        GWSNetworkEdge edge = GWSNetworkEdge( agent->getProperty( EDGE_PROP ).toObject() );

        if( !edge.isValid() ){
            return QString();
        }

        GWSLengthUnit from_distance = coor.getDistance( edge.getFromCoordinate() );
        GWSLengthUnit to_distance = coor.getDistance( edge.getToCoordinate() );
        return from_distance < to_distance ? edge.getFromNodeUID() : edge.getToNodeUID();
    }

    return QString();
}

