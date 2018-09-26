#include "GetToNearestGraphEdgeBehaviour.h"

#include "../../util/geometry/SnapToPointFromEdge.h"
#include "../../skill/move/MoveThroughRouteSkill.h"


GetToNearestGraphEdgeBehaviour::GetToNearestGraphEdgeBehaviour() : GWSBehaviour()
{

}

bool GetToNearestGraphEdgeBehaviour::continueToNext(){

   QSharedPointer<GWSAgent> agent = this->getAgent();

   if ( !agent->getProperty( "closest_edge_point" ) ){
        return false;
   }

}



/**********************************************************************
 METHODS
**********************************************************************/


bool GetToNearestGraphEdgeBehaviour::behave(){

    QSharedPointer< GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coor = ( agent->getProperty( "home_coordX") , agent->getProperty( "home_coordY") );
    GWSCoordinate closest_point_in_edge = SnapToEdgeFromPoint::SnapToEdgeFromPoint( agent_home_coor ).getCoordinates();

    agent->setProperty( "closest_edge_point" , closest_point_in_edge);

    return true;


}
