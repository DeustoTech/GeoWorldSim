#include "SetNextTSPDestinationBehaviour.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString SetNextTSPDestinationBehaviour::TSP_ROUTE_PROPERTY_NAME = "tsp_route";
QString SetNextTSPDestinationBehaviour::STORE_NEXT_DESTINATION_AGENT_ID_AS = "store_next_tsp_agent_id_as";
//QString FollowTSPRouteBehaviour::TSP_ROUTE_STAGE = "tsp_route_stage";
//QString SetNextTSPDestinationBehaviour::STORE_TSP_ROUTE_STAGE_X_AS = "store_tsp_route_stage_x_as";
//QString SetNextTSPDestinationBehaviour::STORE_TSP_ROUTE_STAGE_Y_AS = "store_tsp_route_stage_y_as";
QString SetNextTSPDestinationBehaviour::NEXTS_WHILE_PENDING_ROUTE = "nexts_while_following_route";
QString SetNextTSPDestinationBehaviour::NEXTS_IF_ROUTE_FINISHED = "nexts_if_finished";


SetNextTSPDestinationBehaviour::SetNextTSPDestinationBehaviour() : GWSBehaviour() {
}


QJsonArray SetNextTSPDestinationBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QJsonValue route = agent->getProperty( this->getProperty( TSP_ROUTE_PROPERTY_NAME ).toString() );
    QJsonArray ordered_agents_to_visit_tsp_route_id_array = route.toArray();

    int pending_tsp_elements_size = ordered_agents_to_visit_tsp_route_id_array.size();
    if ( pending_tsp_elements_size > 0 ){

        QString destination_agent_id = ordered_agents_to_visit_tsp_route_id_array.at( 0 ).toString();
        ordered_agents_to_visit_tsp_route_id_array.removeAt( 0 );
        agent->setProperty( this->getProperty( TSP_ROUTE_PROPERTY_NAME ).toString() , ordered_agents_to_visit_tsp_route_id_array );
        agent->setProperty( this->getProperty( STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString() , destination_agent_id );

        QSharedPointer<GWSAgent> destination_agent = GWSAgentEnvironment::globalInstance()->getById( destination_agent_id );
        GWSCoordinate destination_agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( destination_agent )->getCentroid();

        double x = destination_agent_coor.getX();
        double y = destination_agent_coor.getY();

        agent->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_X_PROP , x );
        agent->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_Y_PROP , y );

        return this->getProperty( NEXTS_WHILE_PENDING_ROUTE ).toArray();

       }

      else {

        return this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray();
    }

    return this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray();
}

