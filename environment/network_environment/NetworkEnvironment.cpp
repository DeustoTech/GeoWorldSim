#include "NetworkEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
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

void GWSNetworkEnvironment::deserialize(QJsonObject json){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/*const QSharedPointer<GWSGraphNode> GWSNetworkEnvironment::getNode(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_node.value( agent );
}*/

QSharedPointer<GWSNetworkEdge> GWSNetworkEnvironment::getEdge(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.value( agent );
}

QSharedPointer<GWSAgent> GWSNetworkEnvironment::getAgent(QSharedPointer<GWSNetworkEdge> edge) const{
    if( edge.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.key( edge );
}

QSharedPointer<GWSNetworkEdge> GWSNetworkEnvironment::getEdge( GWSCoordinate from ,  GWSCoordinate to, QString class_name) const{
    QSharedPointer<GWSNetworkEdge> edge = this->getNearestEdge( from , class_name );
    if( edge && edge->getToCoordinate() == to ){
        return edge;
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSNetworkEdge> GWSNetworkEnvironment::getNearestEdge( GWSCoordinate coor , QString class_name ) const{
    if( this->network_edges.keys().contains( class_name ) ){
        return this->network_edges.value( class_name )->getNearestElement<GWSNetworkEdge>( coor );
    }
    return Q_NULLPTR;
}

QPair< GWSCoordinate , QList< QSharedPointer<GWSNetworkEdge> > > GWSNetworkEnvironment::getNearestNodeAndPath(GWSCoordinate coor, QList<GWSCoordinate> get_nearest, QString class_name) const {
    QList< QList< QSharedPointer<GWSNetworkEdge> > > routes_to_all = this->getShortestPaths( coor , get_nearest , class_name );

    Q_ASSERT( routes_to_all.size() == get_nearest.size() );

    // Check which route is shortest
    GWSLengthUnit shortest_length = GWSLengthUnit( 9999999999 );
    QPair< GWSCoordinate , QList< QSharedPointer<GWSNetworkEdge> > > nearest_node_and_route;
    for(int i = 0 ; i < get_nearest.size() ; i++ ){

        QList< QSharedPointer<GWSNetworkEdge> > route = routes_to_all.at( i );
        if( route.isEmpty() ){ continue; }

        GWSLengthUnit l = GWSLengthUnit(0);
        foreach( QSharedPointer<GWSNetworkEdge> e , route ){
            l = l + e->getLength();
        }
        if( l < shortest_length ){
            shortest_length = l;
            nearest_node_and_route = QPair< GWSCoordinate , QList< QSharedPointer<GWSNetworkEdge> > >( get_nearest.at( i ) , route );
        }
    }
    return nearest_node_and_route;
}

QList<QSharedPointer<GWSNetworkEdge> > GWSNetworkEnvironment::getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        // Move given coordinates to real graph nodes
        QString snapped_from = this->getNearestNodeID( from , class_name );
        QString snapped_to = this->getNearestNodeID( to , class_name );
        return this->network_routings.value( class_name )->getShortestPath<GWSNetworkEdge>( snapped_from , snapped_to );
    }
}

QList< QList< QSharedPointer< GWSNetworkEdge> > > GWSNetworkEnvironment::getShortestPath( QList< GWSCoordinate > ordered_coors , QString class_name ) const{

    if( this->network_routings.keys().contains( class_name ) ){
        QStringList snapped_ordered;
        foreach (GWSCoordinate c, ordered_coors) {
            snapped_ordered.append( this->getNearestNodeID( c , class_name ) );
        }
        return this->network_routings.value( class_name )->getShortestPath<GWSNetworkEdge>( snapped_ordered );
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QList< QList< QSharedPointer< GWSNetworkEdge> > >();
}

QList< QList<QSharedPointer< GWSNetworkEdge> > > GWSNetworkEnvironment::getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){

        this->mutex.lock();
        QString snapped_from = this->getNearestNodeID( from_one , class_name );
        QStringList snapped_to_many;
        foreach( GWSCoordinate c , to_many ) {
            snapped_to_many.append( this->getNearestNodeID( c , class_name ) );
        }
        this->mutex.unlock();

        return this->network_routings.value( class_name )->getShortestPaths<GWSNetworkEdge>( snapped_from , snapped_to_many );
    }
    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return QList< QList<QSharedPointer< GWSNetworkEdge> > >();
}


/**********************************************************************
 SETTERS
**********************************************************************/



/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSNetworkEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) || agent->getProperty( EDGE_PROP ).isNull() ){
        return;
    }

    // GRAPH EDGE (comes as a QJSONOBJECT, need to extract it and build the GWSGRAPHEDGE)
    QJsonObject edge_json = agent->getProperty( EDGE_PROP ).toObject();
    QSharedPointer<GWSNetworkEdge> edge = GWSObjectFactory::globalInstance()->fromJSON( edge_json ).dynamicCast<GWSNetworkEdge>();

    if( !edge.isNull() ){

        try {

            GWSEnvironment::registerAgent( agent );
            QJsonArray classes = agent->getInheritanceFamily();
            QStringList keys = this->network_edges.keys();

            foreach(QJsonValue v , classes){

                QString family = v.toString();
                if( family.isEmpty() ){ continue; }

                // Insert new spatial graph with the agents class
                if( !keys.contains( family ) ){

                    this->mutex.lock();
                    this->network_edges.insert( family , QSharedPointer< GWSQuadtree >( new GWSQuadtree() ) );
                    this->network_routings.insert( family , QSharedPointer< GWSRouting >( new GWSRouting() ) );
                    this->mutex.unlock();
                }
            }

            foreach(QJsonValue v , classes){

                QString family = v.toString();
                if( family.isEmpty() ){ continue; }

                // Add to spatial graph
                this->network_edges.value( family )->upsert( edge , edge->getFromCoordinate() );
                this->network_routings.value( family )->upsert( edge );
                this->agent_to_edge.insert( agent , edge );
            }

        } catch (std::exception &e){
            qWarning() << QString("Simulation threw an exception registering agent to GWSNetworkEnvironment. Exception %1").arg( e.what() );
        }
    }

    // Set to null
    agent->setProperty( EDGE_PROP , QJsonValue() );
}

void GWSNetworkEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

    try {

        GWSEnvironment::unregisterAgent( agent );
        QJsonArray classes = agent->getInheritanceFamily();

        foreach(QJsonValue v , classes){

            QString family = v.toString();
            if( family.isEmpty() ){ continue; }

            // Remove from spatial graph
            QSharedPointer<GWSNetworkEdge> edge = this->agent_to_edge.value( agent );
            if( !edge.isNull() ){
                this->network_edges.value( family )->remove( edge );
                this->network_routings.value( family )->remove( edge );
                this->agent_to_edge.remove( agent );
            }
        }

    } catch (std::exception &e){
        qWarning() << QString("Simulation threw an exception unregistering agent from GWSNetworkEnvironment. Exception %1").arg( e.what() );
    }
}

QString GWSNetworkEnvironment::getNearestNodeID( GWSCoordinate coor , QString class_name ) const{
    QSharedPointer<GWSNetworkEdge> edge = this->getNearestEdge( coor , class_name );
    if( !edge ){
        return QString();
    }
    GWSLengthUnit from_distance = coor.getDistance( edge->getFromCoordinate() );
    GWSLengthUnit to_distance = coor.getDistance( edge->getToCoordinate() );
    return from_distance < to_distance ? edge->getFromNodeId() : edge->getToNodeId();
}

