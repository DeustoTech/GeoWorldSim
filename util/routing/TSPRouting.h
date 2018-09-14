#ifndef GSSTSPROUTING_H
#define GSSTSPROUTING_H

#include "../../util/routing/Routing.h"
#include "../../util/routing/DijkstraRouting.h"

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>

class GSSTSPRouting : public GWSRouting
{
    Q_OBJECT

public:

    GSSTSPRouting( QList< QSharedPointer<GWSGraphEdge> > edges );
    ~GSSTSPRouting();

    // METHODS
    QList<GWSCoordinate> nearestNeighborTsp(GWSCoordinate start_node, QList<GWSCoordinate> visit_nodes, GWSCoordinate end_node);
    QList<GWSCoordinate> greedyTsp(GWSCoordinate start_node, QList<GWSCoordinate> visit_nodes, GWSCoordinate end_node);
    QList<GWSCoordinate> insertionTsp(GWSCoordinate start_node, QList<GWSCoordinate> visit_nodes, GWSCoordinate end_node);
    QList<GWSCoordinate> christofidesTsp(GWSCoordinate start_node, QList<GWSCoordinate> visit_nodes, GWSCoordinate end_node);
    QList<GWSCoordinate> opt2Tsp(GWSCoordinate start_node, QList<GWSCoordinate> visit_nodes, GWSCoordinate end_node);
    QList<GWSCoordinate> orderCircularTsp(GWSCoordinate start_node, GWSCoordinate end_node, QList<GWSCoordinate> tsp_circular_nodes);

private:

    GWSDijkstraRouting* dijkstra_routing = 0;


    // Map to relate graph and distance matrix nodes
    QMap<ListDigraph::Node , FullGraph::Node> graph_matrix_nodes;

    void loadDistanceMatrix( FullGraph* distance_matrix , FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSCoordinate> visit_nodes );

};

#endif // GSSTSPROUTING_H
