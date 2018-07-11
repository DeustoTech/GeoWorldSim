#ifndef GSSTSPROUTING_H
#define GSSTSPROUTING_H

#include "utils/routing/Routing.h"
#include "utils/routing/DijkstraRouting.h"

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>

class GSSTSPRouting : public GWSRouting
{
    Q_OBJECT

public:

    GSSTSPRouting( QList<GWSGraphEdge*> edges );
    ~GSSTSPRouting();

    // METHODS
    QList<GWSGraphNode*> nearestNeighborTsp(GWSGraphNode* start_node, QList<GWSGraphNode*> visit_nodes, GWSGraphNode* end_node);
    QList<GWSGraphNode*> greedyTsp(GWSGraphNode* start_node, QList<GWSGraphNode*> visit_nodes, GWSGraphNode* end_node);
    QList<GWSGraphNode*> insertionTsp(GWSGraphNode* start_node, QList<GWSGraphNode*> visit_nodes, GWSGraphNode* end_node);
    QList<GWSGraphNode*> christofidesTsp(GWSGraphNode* start_node, QList<GWSGraphNode*> visit_nodes, GWSGraphNode* end_node);
    QList<GWSGraphNode*> opt2Tsp(GWSGraphNode* start_node, QList<GWSGraphNode*> visit_nodes, GWSGraphNode* end_node);

private:

    GWSDijkstraRouting* dijkstra_routing = 0;


    // Map to relate graph and distance matrix nodes
    QMap<ListDigraph::Node , FullGraph::Node> graph_matrix_nodes;

    void loadDistanceMatrix( FullGraph* distance_matrix , FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSGraphNode*> visit_nodes );
    QList<GWSGraphNode*> orderCircularTsp(GWSGraphNode* start_node, GWSGraphNode* end_node, QList<GWSGraphNode*> tsp_circular_nodes);

};

#endif // GSSTSPROUTING_H
