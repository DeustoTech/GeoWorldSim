#include "TSPRouting.h"

#include "lemon/dijkstra.h"
#include "lemon/nearest_neighbor_tsp.h"
#include "lemon/christofides_tsp.h"
#include "lemon/greedy_tsp.h"
#include "lemon/insertion_tsp.h"
#include "lemon/opt2_tsp.h"

GSSTSPRouting::GSSTSPRouting( QList<GWSGraphEdge*> edges ) : GWSRouting( edges ) {

    // Create dijkstra routing for distances
    this->dijkstra_routing = new GWSDijkstraRouting( edges );

}

GSSTSPRouting::~GSSTSPRouting(){
    // Delete created stuff
    this->dijkstra_routing->deleteLater();
}

/**********************************************************************
 METHODS
**********************************************************************/

QList<GWSGraphNode*> GSSTSPRouting::nearestNeighborTsp(GWSGraphNode *start_node, QList<GWSGraphNode *> visit_nodes, GWSGraphNode *end_node){

    // Distance matrix represented as a graph, create nodes as visit_nodes.size()
    FullGraph* distance_matrix = new FullGraph( visit_nodes.size() );

    // Distance matrix graph costs
    FullGraph::EdgeMap<double>* distance_matrix_weights = new FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_nodes );

    NearestNeighborTsp< FullGraph::EdgeMap<double> >* tsp = new NearestNeighborTsp< FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<GWSGraphNode*> tsp_circular_route_nodes;
    foreach( FullGraph::Node n , tsp->tourNodes() ){
        tsp_circular_route_nodes.append( visit_nodes.at( distance_matrix->id( n ) ) );
    }

    return tsp_circular_route_nodes;// this->orderCircularTsp(start_node , end_node , tsp_circular_route_nodes);
}

QList<GWSGraphNode*> GSSTSPRouting::greedyTsp(GWSGraphNode *start_node, QList<GWSGraphNode *> visit_nodes, GWSGraphNode *end_node){

    // Distance matrix represented as a graph, create nodes as visit_nodes.size()
    FullGraph* distance_matrix = new FullGraph( visit_nodes.size() );

    // Distance matrix graph costs
    FullGraph::EdgeMap<double>* distance_matrix_weights = new FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_nodes );

    GreedyTsp< FullGraph::EdgeMap<double> >* tsp = new GreedyTsp< FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();

    QList<GWSGraphNode*> tsp_circular_route_nodes;
    foreach( FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<GWSGraphNode*> GSSTSPRouting::insertionTsp(GWSGraphNode *start_node, QList<GWSGraphNode *> visit_nodes, GWSGraphNode *end_node){

    // Distance matrix represented as a graph, create nodes as visit_nodes.size()
    FullGraph* distance_matrix = new FullGraph( visit_nodes.size() );

    // Distance matrix graph costs
    FullGraph::EdgeMap<double>* distance_matrix_weights = new FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_nodes );

    InsertionTsp< FullGraph::EdgeMap<double> >* tsp = new InsertionTsp< FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSGraphNode*> tsp_circular_route_nodes;
    foreach( FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

QList<GWSGraphNode*> GSSTSPRouting::christofidesTsp(GWSGraphNode *start_node, QList<GWSGraphNode *> visit_nodes, GWSGraphNode *end_node){

    // Distance matrix represented as a graph, create nodes as visit_nodes.size()
    FullGraph* distance_matrix = new FullGraph( visit_nodes.size() );

    // Distance matrix graph costs
    FullGraph::EdgeMap<double>* distance_matrix_weights = new FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_nodes );

    InsertionTsp< FullGraph::EdgeMap<double> >* tsp = new InsertionTsp< FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSGraphNode*> tsp_circular_route_nodes;
    foreach( FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }
}

QList<GWSGraphNode*> GSSTSPRouting::opt2Tsp(GWSGraphNode *start_node, QList<GWSGraphNode *> visit_nodes, GWSGraphNode *end_node){

    // Distance matrix represented as a graph, create nodes as visit_nodes.size()
    FullGraph* distance_matrix = new FullGraph( visit_nodes.size() );

    // Distance matrix graph costs
    FullGraph::EdgeMap<double>* distance_matrix_weights = new FullGraph::EdgeMap<double>( *distance_matrix );

    this->loadDistanceMatrix( distance_matrix , distance_matrix_weights , visit_nodes );

    InsertionTsp< FullGraph::EdgeMap<double> >* tsp = new InsertionTsp< FullGraph::EdgeMap<double> >( *distance_matrix , *distance_matrix_weights );
    tsp->run();
    QList<GWSGraphNode*> tsp_circular_route_nodes;
    foreach( FullGraph::Node n , tsp->tourNodes() ){
        //tsp_circular_route_nodes.append( this->node_to_original_node.value( this->graph_matrix_nodes.key( n ) ) );
    }

}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GSSTSPRouting::loadDistanceMatrix( FullGraph* distance_matrix , FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSGraphNode*> visit_nodes ){

    // Create all to all distances
    for(int i = 0; i < visit_nodes.size(); i++ ){
        for(int j = 0; j < visit_nodes.size(); j++ ){

            FullGraph::Node from = distance_matrix->nodeFromId( i );
            FullGraph::Node to = distance_matrix->nodeFromId( j );
            FullGraph::Edge edge = distance_matrix->edge( from , to );

            QList<GWSGraphEdge*> route = this->dijkstra_routing->dijkstraShortestPath( visit_nodes.at(i) , visit_nodes.at(j) );
            GWSLengthUnit length = 0;
            foreach( GWSGraphEdge* e , route ){
                length = length + e->getLength();
            }

            distance_matrix_weights->set( edge , length.number() );
        }
    }
}

/**
 * @brief TSPRoutingGraph::orderCircularTsp TSP algorithms return the shortest circular route between all through nodes
 * but don't tell from which node start and end. Order the circular route here
 * @param start_node
 * @param end_node
 * @param tsp_circular_nodes
 * @return
 */
QList<GWSGraphNode*> GSSTSPRouting::orderCircularTsp(GWSGraphNode* start_node, GWSGraphNode* end_node, QList<GWSGraphNode*> tsp_circular_nodes){
    Q_UNUSED(end_node)

    QList<GWSGraphNode*> tsp_ordered_nodes;

    // Find the nearest circular_route node to start_node
    GWSGraphNode* nearest = tsp_circular_nodes.at( 0 );
    double min_distance = start_node->getCoordinate().distance( nearest->getCoordinate() );
    foreach(GWSGraphNode* n , tsp_circular_nodes){
        double d = n->getCoordinate().distance( start_node->getCoordinate() );
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
