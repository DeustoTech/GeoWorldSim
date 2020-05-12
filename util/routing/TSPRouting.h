#ifndef GWSTSPROUTING_H
#define GWSTSPROUTING_H

#include <QObject>
#include <QMutex>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>

#include "../../util/geometry/Coordinate.h"

namespace geoworldsim{
namespace routing {

class TSPRouting : public QObject
{
    Q_OBJECT

public:

    TSPRouting( QString transport_network_type );
    ~TSPRouting();

    // METHODS
    QList<geometry::Coordinate> nearestNeighborTsp(const geometry::Coordinate &start, QList<geometry::Coordinate> visits, const geometry::Coordinate &end);
    QList<geometry::Coordinate> greedyTsp(const geometry::Coordinate &start, QList<geometry::Coordinate> visits, const geometry::Coordinate &end);
    QList<geometry::Coordinate> insertionTsp(const geometry::Coordinate &start, QList<geometry::Coordinate> visits, const geometry::Coordinate &end);
    QList<geometry::Coordinate> christofidesTsp(const geometry::Coordinate &start, QList<geometry::Coordinate> visits, const geometry::Coordinate &end);
    QList<geometry::Coordinate> opt2Tsp(const geometry::Coordinate &start, QList<geometry::Coordinate> visits, const geometry::Coordinate &end);
    QList<geometry::Coordinate> orderCircularTsp(const geometry::Coordinate& start, QList<geometry::Coordinate> tsp_circular_nodes, const geometry::Coordinate& end);

private:

    QMutex mutex;
    QString transport_network_type;

    // Map to relate graph and distance matrix nodes
    QMap<lemon::ListDigraph::Node , lemon::FullGraph::Node> graph_matrix_nodes;

    void loadDistanceMatrix( lemon::FullGraph* distance_matrix , lemon::FullGraph::EdgeMap<double>* distance_matrix_weights , QList<geometry::Coordinate> visit_nodes );

};

}
}

#endif // TSPROUTING_H
