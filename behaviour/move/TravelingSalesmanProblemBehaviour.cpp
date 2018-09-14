#include "TravelingSalesmanProblemBehaviour.h"
#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"


TravelingSalesmanProblemBehaviour::TravelingSalesmanProblemBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool TravelingSalesmanProblemBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    // No moveThroughRoute skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->staticMetaObject.className() ).arg( agent->getId() );
        return false;
    }

    // No destination for MoveThroughRouteSkill
    if( mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP ).isNull() || mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return false;
    }

    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    if ( current_position == mv->getRouteDestination() ){
        // Return true because we have really reached
        return true;
    }

    return false;
}
/**********************************************************************
   METHODS
**********************************************************************/
bool TravelingSalesmanProblemBehaviour::behave(){

     // Tick in 1 second duration to move in small parts
     GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

     QSharedPointer<GWSAgent> agent = this->getAgent();
     GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

      // Generate graph of all GWSAgents in the network environment
      this->setProperty( MoveThroughRouteSkill::EDGES_CLASS_PROP , "ContainerAgent" );
      const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph(this->getProperty(  MoveThroughRouteSkill::EDGES_CLASS_PROP  ).toString());

      // Get all the Edges from the graph
      this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );

      QSharedPointer<GWSGraphEdge> nearest_container_edge = graph->findNearestEdge( agent_coor );
      GWSCoordinate destination_coor = nearest_container_edge->getTo();

      if( agent_coor == destination_coor ){
          return true;
      }

      if( this->route.isEmpty() ){
          // Generate pending route
          this->route = this->routing_graph->dijkstraShortestPath( agent_coor , destination_coor);
      }

      this->setProperty( MoveSkill::DESTINATION_X_PROP , destination_coor.getX() );
      this->setProperty( MoveSkill::DESTINATION_Y_PROP , destination_coor.getY() );

      // Move towards
      // Check if agent can move
      QSharedPointer<MoveThroughRouteSkill> move_throughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
      move_throughroute_skill->move( duration_of_movement );
      emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );


      // Number of villages = number of containers
      //QList< QSharedPointer<GWSAgent> > containers = GWSAgentEnvironment::globalInstance()->getByClass( "ContainerAgent" );
      //int n = containers.size();



      /*if( pending_route.isEmpty() ){ // Assume we have reached route end, free move to destination
          this->setProperty( MoveSkill::DESTINATION_X_PROP , destination_coor.getX() );
          this->setProperty( MoveSkill::DESTINATION_Y_PROP , destination_coor.getY() );
      } else {
          QSharedPointer<GWSGraphEdge> current_edge = pending_route.at(0);

          // Check if we have reached current_edge
          GWSCoordinate current_edge_end = current_edge->getTo();
          this->setProperty( MoveSkill::DESTINATION_X_PROP , current_edge_end.getX() );
          this->setProperty( MoveSkill::DESTINATION_Y_PROP , current_edge_end.getY() );

          if( current_coor == current_edge_end ){
              this->pending_route.removeAt(0);
          }
      }

      MoveSkill::move( movement_duration );*/
    return true;
  }



