#include "CalculateTSPRouteBehaviour.h"

#include "../../skill/move/StoreMultiRouteSkill.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../util/routing/TSPRouting.h"

QString GWSCalculateTSPRouteBehaviour::TSP_AGENT_TYPE = "tsp_agent_type"; // e.g. BUSSTOP
QString GWSCalculateTSPRouteBehaviour::NETWORK_TYPE = "network_type";  // e.g. ROADS
QString GWSCalculateTSPRouteBehaviour::NEXTS = "nexts";

GWSCalculateTSPRouteBehaviour::GWSCalculateTSPRouteBehaviour() : GWSBehaviour(){

}

/*
 * Generate Ordered TSP route with Agent Type input
 */
QPair< double , QJsonArray > GWSCalculateTSPRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();

    GWSGeometry agent_geom = env->getGeometry( agent );
    GWSCoordinate agent_position = agent_geom.getCentroid();

    QString agent_type = this->getProperty( TSP_AGENT_TYPE ).toString();

    // Get agent class to visit depending on user input:
    QList< QSharedPointer<GWSAgent> > agents_to_visit = GWSAgentEnvironment::globalInstance()->getByClass( agent_type ) ;

    // Generate list of containers to visit:
    QList<GWSCoordinate> agents_to_visit_coords;
    QStringList agents_to_visit_id_array;
    QMap< QString , GWSCoordinate > agents_to_visit_coord_id_array;

    foreach ( QSharedPointer<GWSAgent> a , agents_to_visit  ){

        GWSGeometry a_geom = env->getGeometry( a );
        GWSCoordinate agent_to_visit_coord = a_geom.getCentroid();

        QString agent_to_visit_id = a->getUID();
        agents_to_visit_coords.append( agent_to_visit_coord ) ;
        agents_to_visit_id_array.append( agent_to_visit_id );
        agents_to_visit_coord_id_array.insert( agent_to_visit_id , agent_to_visit_coord );

    }

    // Generate TSP algorithm with those edges
    GWSTSPRouting* routing = new GWSTSPRouting( this->getProperty( NETWORK_TYPE ).toString() );

    // Get nearest neighbour given start coordinates and containers to visit
    QList< GWSCoordinate > agents_to_visit_route_coord_array = routing->nearestNeighborTsp( agent_position , agents_to_visit_coords , agent_position );

    // Order the nodes to get best route. This is the route to follow.
    QList< GWSCoordinate > ordered_agents_to_visit_tsp_route_coord_array = routing->orderCircularTsp( agent_position , agent_position , agents_to_visit_route_coord_array );

    // Compare the coordinates of the ordered route and those in the initial QPair
    // QList of coors and IDs to extract the ID of the ordered coors

    QSharedPointer<GWSStoreMultiRouteSkill> multiroute_skill = agent->getSkill( GWSStoreMultiRouteSkill::staticMetaObject.className() ).dynamicCast<GWSStoreMultiRouteSkill>();
    if( !multiroute_skill ){
        multiroute_skill = QSharedPointer<GWSStoreMultiRouteSkill>( new GWSStoreMultiRouteSkill() );
        agent->addSkill( multiroute_skill );
    }

    // Extract array of ordered nodes IDs:
    foreach( GWSCoordinate coor , ordered_agents_to_visit_tsp_route_coord_array ){

        QString id = agents_to_visit_coord_id_array.key( coor );
        multiroute_skill->addDestination( id , coor );
    }

    //TODO routing->deleteLater();
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}



