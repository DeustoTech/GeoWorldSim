#include "SetNextTSPDestinationBehaviour.h"

#include "../../skill/move/CalculateTSPSkill.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString SetNextTSPDestinationBehaviour::STORE_NEXT_TSP_DESTINATION_X_VALUE = "next_x_value";
QString SetNextTSPDestinationBehaviour::STORE_NEXT_TSP_DESTINATION_Y_VALUE = "next_y_value";
QString SetNextTSPDestinationBehaviour::STORE_NEXT_DESTINATION_AGENT_ID_AS = "next_agent_id";
QString SetNextTSPDestinationBehaviour::NEXTS_WHILE_PENDING_ROUTE = "nexts_while_pending_route";
QString SetNextTSPDestinationBehaviour::NEXTS_IF_ROUTE_FINISHED = "nexts_if_finished";

SetNextTSPDestinationBehaviour::SetNextTSPDestinationBehaviour() : GWSBehaviour() {
}


QJsonArray SetNextTSPDestinationBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QJsonValue route = agent->getProperty( CalculateTSPSkill::AGENT_PENDING_TSP_ROUTE_ELEMENTS );
    QJsonArray ordered_agents_to_visit_tsp_route_id_array = route.toArray();

    int pending_tsp_elements_size = ordered_agents_to_visit_tsp_route_id_array.size();
    if ( pending_tsp_elements_size > 0 ){

        QString destination_agent_id = ordered_agents_to_visit_tsp_route_id_array.at( 0 ).toString();
        ordered_agents_to_visit_tsp_route_id_array.removeAt( 0 );
        agent->setProperty( CalculateTSPSkill::AGENT_PENDING_TSP_ROUTE_ELEMENTS , ordered_agents_to_visit_tsp_route_id_array );
        agent->setProperty( this->getProperty( STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString() , destination_agent_id );

        QSharedPointer<GWSAgent> destination_agent = GWSAgentEnvironment::globalInstance()->getById( destination_agent_id );
        GWSCoordinate destination_agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( destination_agent )->getCentroid();

        double x = destination_agent_coor.getX();
        double y = destination_agent_coor.getY();
        agent->setProperty( this->getProperty( STORE_NEXT_TSP_DESTINATION_X_VALUE).toString() , x );
        agent->setProperty( this->getProperty( STORE_NEXT_TSP_DESTINATION_Y_VALUE).toString() , y );

        return this->getProperty( NEXTS_WHILE_PENDING_ROUTE ).toArray();

    } else {

        return this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray();
    }

    return this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray();
}

