#include "CheckIfAtClosestEdgePointBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

CheckIfAtClosestEdgePointBehaviour::CheckIfAtClosestEdgePointBehaviour() : GWSBehaviour()
{

}

bool CheckIfAtClosestEdgePointBehaviour::continueToNext(){

    QSharedPointer< GWSAgent> agent = this->getAgent();
    double closestEdgeX = agent->getProperty( "closest_edge_pointX" ).toDouble();
    double closestEdgeY = agent->getProperty( "closest_edge_pointY" ).toDouble();
    GWSCoordinate closestEdgeCoor = GWSCoordinate( closestEdgeX, closestEdgeY);

    if( !closestEdgeCoor.isValid() ){
        return false;
    }

    GWSPhysicalEnvironment * env = GWSPhysicalEnvironment::globalInstance();
    return env->getGeometry( this->getAgent() )->getCentroid() == closestEdgeCoor;
}


bool CheckIfAtClosestEdgePointBehaviour::behave(){
    // EMPTY, DO NOTHING
    return true;
}
