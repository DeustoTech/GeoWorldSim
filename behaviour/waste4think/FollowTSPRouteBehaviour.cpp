#include "FollowTSPRouteBehaviour.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString FollowTSPRouteBehaviour::TSP_ROUTE = "tsp_route";
QString FollowTSPRouteBehaviour::STORE_CLOSEST_ID_AS = "store_closest_id_as";
QString FollowTSPRouteBehaviour::TSP_ROUTE_STAGE = "tsp_route_stage";
QString FollowTSPRouteBehaviour::STORE_TSP_ROUTE_STAGE_X_AS = "store_tsp_route_stage_x_as";
QString FollowTSPRouteBehaviour::STORE_TSP_ROUTE_STAGE_Y_AS = "store_tsp_route_stage_y_as";
QString FollowTSPRouteBehaviour::NEXTS = "nexts";


FollowTSPRouteBehaviour::FollowTSPRouteBehaviour() : GWSBehaviour() {
}


QStringList FollowTSPRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QVariant route = this->getProperty( TSP_ROUTE );

    QStringList tsp_route = route.toStringList();
    int route_size = tsp_route.size();

    int loop_stage = this->getProperty( TSP_ROUTE_STAGE ).toInt();

    if ( loop_stage <= ( route_size - 1 )  ){

       QSharedPointer< GWSAgent > tsp_stage_agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( "ContainerAgent" ,  tsp_route[ loop_stage ]);
       GWSCoordinate tsp_stage_agent_coors = GWSPhysicalEnvironment::globalInstance()->getGeometry( tsp_stage_agent )->getCentroid();

       double x = tsp_stage_agent_coors.getX();
       double y = tsp_stage_agent_coors.getY();

       agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_X_AS ).toString() , x );
       agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_Y_AS ).toString() , y );
       agent->setProperty( this->getProperty( STORE_CLOSEST_ID_AS ).toString() , tsp_stage_agent->getId() );
       loop_stage = loop_stage + 1;

       }
      else {
       agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_X_AS ).toString() , agent->getProperty( "home_x" ).toDouble() );
       agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_Y_AS ).toString() , agent->getProperty( "home_y" ).toDouble() );
       loop_stage = 0;
    }

    this->setProperty( TSP_ROUTE_STAGE  , loop_stage );
    agent->setProperty( TSP_ROUTE_STAGE  , loop_stage );

    return this->getNexts( NEXTS );
}

