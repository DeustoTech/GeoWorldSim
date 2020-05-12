#include "TSPRouting.h"

#include "lemon/dijkstra.h"
#include "lemon/nearest_neighbor_tsp.h"
#include "lemon/christofides_tsp.h"
#include "lemon/greedy_tsp.h"
#include "lemon/insertion_tsp.h"
#include "lemon/opt2_tsp.h"

#include <stdlib.h>

#include "../../environment/network_environment/NetworkEdge.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"

using namespace geoworldsim;

geoworldsim::routing::TSPRouting::TSPRouting( QString transport_network_type ) : QObject() {
    this->transport_network_type = transport_network_type;
}

geoworldsim::routing::TSPRouting::~TSPRouting(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::nearestNeighborTsp(const geometry::Coordinate& start_coor , QList<geometry::Coordinate > visit_coordinates , const geometry::Coordinate& end_coor){

    this->mutex.lock();

    visit_coordinates.append( end_coor );
    visit_coordinates.prepend( start_coor );

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::NearestNeighborTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::NearestNeighborTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<geometry::Coordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        tsp_circular_route_nodes.append( visit_coordinates.at( distance_matrix->id( n ) ) );
    }

    this->mutex.unlock();

    return tsp_circular_route_nodes;// this->orderCircularTsp(start_coordinate , end_coordinate , tsp_circular_route_nodes);
}

QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::greedyTsp(const geometry::Coordinate& start_coor , QList<geometry::Coordinate> visit_coordinates , const geometry::Coordinate& end_coor){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::GreedyTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::GreedyTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<geometry::Coordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::insertionTsp(const geometry::Coordinate& start_coor , QList<geometry::Coordinate> visit_coordinates , const geometry::Coordinate& end_coor){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<geometry::Coordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::christofidesTsp(const geometry::Coordinate &start_coordinate, QList<geometry::Coordinate> visit_coordinates, const geometry::Coordinate &end_coordinate){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<geometry::Coordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }
}

QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::opt2Tsp(const geometry::Coordinate& start_coor , QList<geometry::Coordinate> visit_coordinates , const geometry::Coordinate& end_coor){

    // Distance matrix represented as a graph, create nodes as visit_coordinates.size()
    lemon::FullGraph* distance_matrix = new lemon::FullGraph( visit_coordinates.size() );

    // Distance matrix graph costs
    lemon::FullGraph::EdgeMap<double>* distance_matrix_weights = new lemon::FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_coordinates );

    lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >* tsp = new lemon::InsertionTsp< lemon::FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<geometry::Coordinate> tsp_circular_route_nodes;
    foreach( lemon::FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

/**********************************************************************
 PRIVATE
**********************************************************************/

void geoworldsim::routing::TSPRouting::loadDistanceMatrix( lemon::FullGraph* distance_matrix , lemon::FullGraph::EdgeMap<double>* distance_matrix_weights , QList<geometry::Coordinate> visit_coordinates ){

    if( this->transport_network_type.isEmpty() ){
        return;
    }

    // Create all to all distances
    for(int i = 0; i < visit_coordinates.size(); i++ ){

        QList< QStringList > routes_from_i = environment::NetworkEnvironment::globalInstance()->getShortestPaths( visit_coordinates.at(i) , visit_coordinates , this->transport_network_type );
        for(int j = 0; j < routes_from_i.size(); j++ ){

            lemon::FullGraph::Node from = distance_matrix->nodeFromId( i );
            lemon::FullGraph::Node to = distance_matrix->nodeFromId( j );
            lemon::FullGraph::Edge edge = distance_matrix->edge( from , to );

            QList< QSharedPointer<Entity> > route_edges = environment::EntityEnvironment::globalInstance()->getByUIDS( routes_from_i.at(j) );

            unit::LengthUnit length = 0;
            foreach( QSharedPointer<Entity> e , route_edges ){
                length = length + graph::NetworkEdge( e->getProperty( environment::NetworkEnvironment::EDGE_PROP ).toObject() ).getCost();
            }
            if( length <= unit::LengthUnit( 0 ) ){
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
QList<geometry::Coordinate> geoworldsim::routing::TSPRouting::orderCircularTsp(const geometry::Coordinate& start, QList<geometry::Coordinate> tsp_circular_nodes, const geometry::Coordinate& end){
    Q_UNUSED(end)

    QList<geometry::Coordinate> tsp_ordered_nodes;

    // Find the nearest circular_route node to start_coordinate
    geometry::Coordinate nearest = tsp_circular_nodes.at( 0 );
    unit::LengthUnit min_distance = start.getDistance( nearest );
    foreach(geometry::Coordinate n , tsp_circular_nodes){
        unit::LengthUnit d = n.getDistance( start );
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
