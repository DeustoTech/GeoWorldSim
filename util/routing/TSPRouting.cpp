#include "TSPRouting.h"

#include "lemon/dijkstra.h"
#include "lemon/nearest_neighbor_tsp.h"
#include "lemon/christofides_tsp.h"
#include "lemon/greedy_tsp.h"
#include "lemon/insertion_tsp.h"
#include "lemon/opt2_tsp.h"

#include "../../environment/network_environment/NetworkEnvironment.h"

GWSTSPRouting::GWSTSPRouting( QString transport_network_type ) : QObject() {
    this->transport_network_type = transport_network_type;
}

GWSTSPRouting::~GWSTSPRouting(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QList<GWSCoordinate> GWSTSPRouting::nearestNeighborTsp(GWSCoordinate start_coor, QList<GWSCoordinate > visit_coordinates, GWSCoordinate end_coor){

    this->mutex.lock();

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::NearestNeighborTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::NearestNeighborTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<GWSCoordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        tsp_circular_route_nodes.append( visit_coordinates.at( distance_matrix->id( n ) ) );
    }

    this->mutex.unlock();

    return tsp_circular_route_nodes;// this->orderCircularTsp(start_coordinate , end_coordinate , tsp_circular_route_nodes);
}

QList<GWSCoordinate> GWSTSPRouting::greedyTsp(GWSCoordinate start_coordinate, QList<GWSCoordinate > visit_coordinates, GWSCoordinate end_coordinate){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::GreedyTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::GreedyTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<GWSCoordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<GWSCoordinate> GWSTSPRouting::insertionTsp(GWSCoordinate start_coordinate, QList<GWSCoordinate > visit_coordinates, GWSCoordinate end_coordinate){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSCoordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<GWSCoordinate> GWSTSPRouting::christofidesTsp(GWSCoordinate start_coordinate, QList<GWSCoordinate > visit_coordinates, GWSCoordinate end_coordinate){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSCoordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }
}

QList<GWSCoordinate> GWSTSPRouting::opt2Tsp(GWSCoordinate start_coordinate, QList<GWSCoordinate > visit_coordinates, GWSCoordinate end_coordinate){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSCoordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSTSPRouting::loadDistanceMatrix( lemon::FullGraph* distance_matrix , lemon::FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSCoordinate> visit_coordinates ){

    if( this->transport_network_type.isEmpty() ){
        return;
    }

    // Create all to all distances
    for(int i = 0; i < visit_coordinates.size(); i++ ){

        QList< QList< QSharedPointer <GWSNetworkEdge> > > routes = GWSNetworkEnvironment::globalInstance()->getShortestPaths( visit_coordinates.at(i) , visit_coordinates , this->transport_network_type );
        for(int j = 0; j < visit_coordinates.size(); j++ ){

            lemon::FullGraph::Node from = distance_matrix->nodeFromId( i );
            lemon::FullGraph::Node to = distance_matrix->nodeFromId( j );
            lemon::FullGraph::Edge edge = distance_matrix->edge( from , to );

            QList< QSharedPointer <GWSNetworkEdge> > route = routes.at(j);

            GWSLengthUnit length = 0;
            foreach( QSharedPointer <GWSNetworkEdge> e , route ){
                length = length + e->getLength();
            }
            if( length <= GWSLengthUnit( 0 ) ){
                length = visit_coordinates.at(i).getDistance( visit_coordinates.at(j) );
            }

            distance_matrix_weights->set( edge , length.number() );
        }
    }
}

/**
 * @brief TSPRoutingGraph::orderCircularTsp TSP algorithms return the shortest circular route between all through nodes
 * but don't tell from which node start and end. Order the circular route here
 * @param start_coordinate
 * @param end_coordinate
 * @param tsp_circular_nodes
 * @return
 */
QList<GWSCoordinate> GWSTSPRouting::orderCircularTsp(GWSCoordinate start_coordinate, GWSCoordinate end_coordinate, QList<GWSCoordinate> tsp_circular_nodes){
    Q_UNUSED(end_coordinate)

    QList<GWSCoordinate> tsp_ordered_nodes;

    // Find the nearest circular_route node to start_coordinate
    GWSCoordinate nearest = tsp_circular_nodes.at( 0 );
    GWSLengthUnit min_distance = start_coordinate.getDistance( nearest );
    foreach(GWSCoordinate n , tsp_circular_nodes){
        GWSLengthUnit d = n.getDistance( start_coordinate );
        if( min_distance > d ){
            nearest = n;
            min_distance = d;
        }
    }

    // Now order the circular route starting from the nearest node
    int nearest_index = tsp_circular_nodes.indexOf( nearest );
    for(int i = 0; i < tsp_circular_nodes.size() ; i++){
        tsp_ordered_nodes.append( tsp_circular_nodes.at( (nearest_index + i) % tsp_circular_nodes.size() ) );
    }

    return tsp_ordered_nodes;
}
