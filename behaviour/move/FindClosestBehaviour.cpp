#include "FindClosestBehaviour.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/view/ViewSkill.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../util/routing/DijkstraRouting.h"

#include "ContainerAgent.h"


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour(){
}


bool FindClosestBehaviour::continueToNext(){

    return false;
}


void FindClosestBehaviour::generateGraph(){
    // Generate graph of all GWSAgents in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( "GWSAgent" );

    // Get all the Edges from the graph
    this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );
}




bool FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coord = GWSCoordinate( agent->getProperty( "home_coordX" ).toDouble(), agent->getProperty( "home_coordY" ).toDouble() );

    //GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
    //QSharedPointer<GWSAgent> ClosestContainer = env->getNearestAgent( agent_coord , "ContainerAgent" );

    QMap<QString , GWSCoordinate > container_coord_id_array;
    QList < GWSCoordinate > container_coord_array;

    QList<QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className()) ;
    foreach ( QSharedPointer<GWSAgent> a, agents  ){
         GWSCoordinate container_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
         QString container_id = a->getProperty("@id").toString();
         container_coord_array.append( container_coord ) ;
         container_coord_id_array.insert( container_id , container_coord );
     }

    if( !this->routing_graph ){
        this->generateGraph();
    }

    this->closest_coor = this->routing_graph->dijkstraNearestNode( agent_home_coord , container_coord_array);
    QString closest_id = container_coord_id_array.key( this->closest_coor );
    agent->setProperty( "compare_agent_id" , closest_id );
    agent->color = QColor("red");

    //double ClosestContainer_coordX = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainer )->getCentroid().getX();
    //double ClosestContainer_coordY = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainer )->getCentroid().getY();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , this->closest_coor.getX() );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , this->closest_coor.getY() );

    return true;

}
