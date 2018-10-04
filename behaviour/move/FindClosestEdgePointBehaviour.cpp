#include "FindClosestEdgePointBehaviour.h"

#include "../../util/geometry/SnapToEdgeFromPoint.h"
#include "../../skill/move/MoveThroughRouteSkill.h"


FindClosestEdgePointBehaviour::FindClosestEdgePointBehaviour() : GWSBehaviour()
{

}

bool FindClosestEdgePointBehaviour::continueToNext(){

   QSharedPointer<GWSAgent> agent = this->getAgent();

   if ( agent->getProperty( "closest_edge_point" ).isNull() ){
        return false;
   }

   return true;

}



/**********************************************************************
 METHODS
**********************************************************************/


bool FindClosestEdgePointBehaviour::behave(){

    QSharedPointer< GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );
    SnapToEdgeFromPoint *snap = new SnapToEdgeFromPoint( agent_home_coor );
    GWSCoordinate closest_point_in_edge = snap->getCoordinates();

    agent->setProperty( "closest_edge_pointX" , closest_point_in_edge.getX());
    agent->setProperty( "closest_edge_pointY" , closest_point_in_edge.getY());

    return true;


}
