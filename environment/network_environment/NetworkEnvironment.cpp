#include "NetworkEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

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

QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getEdge(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.value( agent );
}

QSharedPointer<GWSAgent> GWSNetworkEnvironment::getAgent(QSharedPointer<GWSGraphEdge> edge) const{
    if( edge.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.key( edge );
}

QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getEdge( GWSCoordinate from,  GWSCoordinate to, QString class_name) const{
    QSharedPointer<GWSGraphEdge> edge = this->getNearestEdge( from , class_name );
    if( edge && edge->getTo() == to ){
        return edge;
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getNearestEdge( GWSCoordinate coor , QString class_name) const{
    if( this->network_edges.keys().contains( class_name ) ){
        return this->network_edges.value( class_name )->getNearestElement<GWSGraphEdge>( coor );
    }
    return Q_NULLPTR;
}

GWSCoordinate GWSNetworkEnvironment::getNearestNode( GWSCoordinate coor , QString class_name) const{
    QSharedPointer<GWSGraphEdge> edge = this->getNearestEdge( coor , class_name );
    if( !edge ){
        return GWSCoordinate();
    }
    GWSLengthUnit from_distance = coor.getDistance( edge->getFrom() );
    GWSLengthUnit to_distance = coor.getDistance( edge->getTo() );
    return from_distance < to_distance ? edge->getFrom() : edge->getTo();
}

QPair< GWSCoordinate , QList< QSharedPointer<GWSGraphEdge> > > GWSNetworkEnvironment::getNearestNodeAndPath(GWSCoordinate coor, QList<GWSCoordinate> get_nearest, QString class_name) const {
    QList< QList< QSharedPointer<GWSGraphEdge> > > routes_to_all = this->getShortestPaths( coor , get_nearest , class_name );

    Q_ASSERT( routes_to_all.size() == get_nearest.size() );

    // Check which route is shortest
    GWSLengthUnit shortest_length = GWSLengthUnit( 9999999999 );
    QPair< GWSCoordinate , QList< QSharedPointer<GWSGraphEdge> > > nearest_node_and_route;
    for(int i = 0 ; i < get_nearest.size() ; i++ ){

        QList< QSharedPointer<GWSGraphEdge> > route = routes_to_all.at( i );
        if( route.isEmpty() ){ continue; }

        GWSLengthUnit l = GWSLengthUnit(0);
        foreach( QSharedPointer<GWSGraphEdge> e , route ){
            l = l + e->getLength();
        }
        if( l < shortest_length ){
            shortest_length = l;
            nearest_node_and_route = QPair< GWSCoordinate , QList< QSharedPointer<GWSGraphEdge> > >( get_nearest.at( i ) , route );
        }
    }
    return nearest_node_and_route;
}

QList<QSharedPointer<GWSGraphEdge> > GWSNetworkEnvironment::getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        // Move given coordinates to real graph nodes
        GWSCoordinate snapped_from_coor = this->getNearestNode( from , class_name );
        GWSCoordinate snapped_to_coor = this->getNearestNode( to , class_name );
        return this->network_routings.value( class_name )->getShortestPath( snapped_from_coor , snapped_to_coor );
    }
}

QList<QList<QSharedPointer<GWSGraphEdge> > > GWSNetworkEnvironment::getShortestPath(QList< GWSCoordinate > ordered_coors , QString class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        QList< GWSCoordinate > snapped_ordered_coors;
        foreach (GWSCoordinate c, ordered_coors) {
            snapped_ordered_coors.append( this->getNearestNode( c , class_name ) );
        }
        return this->network_routings.value( class_name )->getShortestPath( snapped_ordered_coors );
    }
}

QList<QList<QSharedPointer< GWSGraphEdge> > > GWSNetworkEnvironment::getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const{
    if( this->network_routings.keys().contains( class_name ) ){
        GWSCoordinate snapped_from_coor = this->getNearestNode( from_one , class_name );
        QList< GWSCoordinate > snapped_to_many_coors;
        foreach (GWSCoordinate c, to_many) {
            snapped_to_many_coors.append( this->getNearestNode( c , class_name ) );
        }
        return this->network_routings.value( class_name )->getShortestPaths( snapped_from_coor , snapped_to_many_coors );
    }
}


/**********************************************************************
 SETTERS
**********************************************************************/



/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSNetworkEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) ){
        return;
    }

    // GRAPH EDGE (comes parsed by GWSObject, extract and set it to null)
    QSharedPointer<GWSGraphEdge> edge = agent->getProperty( EDGE_PROP ).value< QSharedPointer<GWSObject> >().dynamicCast<GWSGraphEdge>();

    if( !edge.isNull() ){

        try {

            GWSEnvironment::registerAgent( agent );
            QStringList classes = agent->getInheritanceFamily();
            QList<QString> keys = this->network_edges.keys();

            foreach(QString family , classes){

                // Insert new spatial graph with the agents class
                if( !keys.contains( family ) ){

                    this->mutex.lock();
                    this->network_edges.insert( family , QSharedPointer< GWSQuadtree >( new GWSQuadtree( family + "-network-env-index" ) ) );
                    this->network_routings.insert( family , QSharedPointer< GWSRouting >( new GWSRouting() ) );
                    this->mutex.unlock();
                }
            }

            foreach(QString family , classes){

                // Add to spatial graph
                this->network_edges.value( family )->upsert( edge , edge->getFrom() );
                this->network_routings.value( family )->upsert( edge );
                this->agent_to_edge.insert( agent , edge );
            }

        } catch (std::exception &e){
            emit GWSApp::globalInstance()->sendAlertToSocketSignal( 1 , "Catched exception" , QString("Simulation threw an exception registering agent to GWSNetworkEnvironment. Exception %1").arg( e.what() ) );
        }
    }

    // Set to null
    agent->setProperty( EDGE_PROP , QVariant() );
}

void GWSNetworkEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

    try {

        GWSEnvironment::unregisterAgent( agent );
        QStringList classes = agent->getInheritanceFamily();

        foreach(QString family , classes){

            // Remove from spatial graph
            QSharedPointer<GWSGraphEdge> edge = this->agent_to_edge.value( agent );
            if( !edge.isNull() ){
                this->network_edges.value( family )->remove( edge );
                this->network_routings.value( family )->remove( edge );
                this->agent_to_edge.remove( agent );
            }
        }

    } catch (std::exception &e){
        emit GWSApp::globalInstance()->sendAlertToSocketSignal( 1 , "Catched exception" , QString("Simulation threw an exception unregistering agent from GWSNetworkEnvironment. Exception %1").arg( e.what() ) );
    }
}
