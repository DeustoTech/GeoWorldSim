#ifndef GWSTSPROUTING_H
#define GWSTSPROUTING_H

#include <QObject>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>

#include "../../util/graph/GraphEdge.h"
#include "../../util/geometry/Coordinate.h"

class GWSTSPRouting : public QObject
{
    Q_OBJECT

public:

    GWSTSPRouting( QString transport_network_type );
    ~GWSTSPRouting();

    // METHODS
    QList<GWSCoordinate> nearestNeighborTsp(GWSCoordinate start, QList<GWSCoordinate> visits, GWSCoordinate end);
    QList<GWSCoordinate> greedyTsp(GWSCoordinate start, QList<GWSCoordinate> visits, GWSCoordinate end);
    QList<GWSCoordinate> insertionTsp(GWSCoordinate start, QList<GWSCoordinate> visits, GWSCoordinate end);
    QList<GWSCoordinate> christofidesTsp(GWSCoordinate start, QList<GWSCoordinate> visits, GWSCoordinate end);
    QList<GWSCoordinate> opt2Tsp(GWSCoordinate start, QList<GWSCoordinate> visits, GWSCoordinate end);
    QList<GWSCoordinate> orderCircularTsp(GWSCoordinate start, GWSCoordinate end, QList<GWSCoordinate> tsp_circular_nodes);

private:

    QString transport_network_type;

    // Map to relate graph and distance matrix nodes
    QMap<lemon::ListDigraph::Node , lemon::FullGraph::Node> graph_matrix_nodes;

    void loadDistanceMatrix( lemon::FullGraph* distance_matrix , lemon::FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSCoordinate> visit_nodes );

};

#endif // GWSTSPROUTING_H
