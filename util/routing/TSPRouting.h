#ifndef GWSTSPROUTING_H
#define GWSTSPROUTING_H

#include <QObject>
#include <QMutex>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>

#include "../../util/geometry/Coordinate.h"

class GWSTSPRouting : public QObject
{
    Q_OBJECT

public:

    GWSTSPRouting( QString transport_network_type );
    ~GWSTSPRouting();

    // METHODS
    QList<GWSCoordinate> nearestNeighborTsp(const GWSCoordinate &start, QList<GWSCoordinate> visits, const GWSCoordinate &end);
    QList<GWSCoordinate> greedyTsp(const GWSCoordinate &start, QList<GWSCoordinate> visits, const GWSCoordinate &end);
    QList<GWSCoordinate> insertionTsp(const GWSCoordinate &start, QList<GWSCoordinate> visits, const GWSCoordinate &end);
    QList<GWSCoordinate> christofidesTsp(const GWSCoordinate &start, QList<GWSCoordinate> visits, const GWSCoordinate &end);
    QList<GWSCoordinate> opt2Tsp(const GWSCoordinate &start, QList<GWSCoordinate> visits, const GWSCoordinate &end);
    QList<GWSCoordinate> orderCircularTsp(const GWSCoordinate& start, QList<GWSCoordinate> tsp_circular_nodes, const GWSCoordinate& end);

private:

    QMutex mutex;
    QString transport_network_type;

    // Map to relate graph and distance matrix nodes
    QMap<lemon::ListDigraph::Node , lemon::FullGraph::Node> graph_matrix_nodes;

    void loadDistanceMatrix( lemon::FullGraph* distance_matrix , lemon::FullGraph::EdgeMap<double>* distance_matrix_weights , QList<GWSCoordinate> visit_nodes );

};

#endif // GWSTSPROUTING_H
