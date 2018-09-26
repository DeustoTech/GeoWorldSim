#include "SnapToPointFromEdge.h"

#include "../../util/graph/Graph.h"
#include "../../util/graph/GraphEdge.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include <QtMath>

SnapToEdgeFromPoint::SnapToEdgeFromPoint( GWSCoordinate home_coor )  {

    /*
     * Returns the coordinates of the closest point in the nearest edge w.r.t. the agent
     */

    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );
    QSharedPointer<GWSGraphEdge> nearest_edge = graph->findNearestEdge( home_coor );

    GWSCoordinate edge_start = nearest_edge->getFrom();
    double x1 = edge_start.getX();
    double y1 = edge_start.getY();

    GWSCoordinate edge_end = nearest_edge->getTo();
    double x2 = edge_end.getX();
    double y2 = edge_end.getY();

    double x3 = home_coor.getX();
    double y3 = home_coor.getY();

    double zatikizuna = x3 * qPow( x2 - x1 , 2 ) - y3 * ( x2 - x1 ) * ( y1 - y2) + x2 * qPow( y1 , 2 ) - x1 * y1 * y2 - x2 * y1 * y2 + x1 * qPow( y2 , 2 ) ;
    double zatitzailea =  qPow( x2 - x1 , 2 ) + qPow( y1 - y2 , 2 ) ;

    double x = zatikizuna / zatitzailea;
    double y = ( (y2 - y1) * x + x2 * y1 - x1 * y2 )/ ( x2 - x1 );

    this->coors_to_snap_to = GWSCoordinate( x , y );

}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSCoordinate SnapToEdgeFromPoint::getCoordinates(  ){
    return this->coors_to_snap_to;
}

