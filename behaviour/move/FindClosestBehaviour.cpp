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

FindClosestBehaviour::~FindClosestBehaviour(){
    this->routing_graph->deleteLater();
}

bool FindClosestBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QVariant closest_agent_id = agent->getProperty( "closest_found_id" );

    // If there is no closest found agent assigned, go to behave:
    if( closest_agent_id.isNull() ){
        return false;
    }

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    GWSCoordinate destination_coor = mv->getRouteDestination();

    QSharedPointer<GWSAgent> closest_agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( ContainerAgent::staticMetaObject.className(), closest_agent_id.toString() );
    GWSCoordinate closest_agent_coors = GWSPhysicalEnvironment::globalInstance()->getGeometry( closest_agent )->getCentroid();

    // If the destination is not that of the closest agent, behave:
    if( destination_coor != closest_agent_coors){
        return false;
    }
    return true;
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
    agent->setProperty( "closest_found_id" , closest_id );

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , this->closest_coor.getX() );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , this->closest_coor.getY() );

    return true;

}
