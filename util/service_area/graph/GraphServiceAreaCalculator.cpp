#include "GraphServiceAreaCalculator.h"

#include "geos/geom/LineString.h"
#include "geos/simplify/TopologyPreservingSimplifier.h"

GraphServiceAreaCalculator::GraphServiceAreaCalculator(GWSObject *parent) : ServiceAreaCalculator( parent ){

}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<const GWSGraphEdge*> GraphServiceAreaCalculator::getReachableEdges(const GWSGraph *graph, const GWSGraphNode *from, double cost){

    QList<const GWSGraphNode*>* visited_nodes_ref = new QList<const GWSGraphNode*>();
    QList<const GWSGraphEdge*>* reachable_edges_ref = new QList<const GWSGraphEdge*>();
    this->getReachableEdgesRecursive( graph , from , cost , visited_nodes_ref , reachable_edges_ref);

    QList<const GWSGraphEdge*> reachable_edges;
    foreach( const GWSGraphEdge* edge , *reachable_edges_ref ){
        reachable_edges.append( edge );
    }

    delete reachable_edges_ref;
    delete visited_nodes_ref;
    return reachable_edges;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

/**
 * WARNING SOME EDGES MAY APPEAR MORE THAN ONCE IN THE RETURNED LIST
 * @brief GraphServiceAreaCalculator::getReachableEdges
 * @param graph
 * @param from
 * @param cost
 * @param visited_nodes
 * @return
 */
void GraphServiceAreaCalculator::getReachableEdgesRecursive(const GWSGraph *graph, const GWSGraphNode *from, double cost, QList<const GWSGraphNode *>* visited_nodes , QList<const GWSGraphEdge *>* reachable_edges){

    visited_nodes->append( from );
    foreach( GWSGraphEdge* edge , from->getDepartingEdges() ){

        double remaining_cost = cost - edge->getCost();
        GWSGraphNode* to_node = edge->getToNode();

        if( remaining_cost > 0 ){

            if( !reachable_edges->contains( edge ) ){ reachable_edges->append( edge ); }
            if( !visited_nodes->contains( to_node ) ){ this->getReachableEdgesRecursive( graph , to_node , remaining_cost , visited_nodes , reachable_edges ); }
        }
    }

}
