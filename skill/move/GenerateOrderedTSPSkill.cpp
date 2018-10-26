#include "GenerateOrderedTSPSkill.h"

#include "../../skill/Skill.h"

QString GenerateOrderedTSPSkill::VISIT_AGENTS_PROP = "visit_agents_type";


GenerateOrderedTSPSkill::GenerateOrderedTSPSkill() : GWSSkill(){

}


/**********************************************************************
 GETTERS
**********************************************************************/

QList< QPair< GWSCoordinate , QString > > GenerateOrderedTSPSkill::generateOrderedTSP(){

        QSharedPointer<GWSAgent> agent = this->getAgent();
        GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX" ).toDouble(), agent->getProperty( "home_coordY" ).toDouble() );

        // Get agent class to visit:
        QString agent_type_to_visit = this->getProperty( VISIT_AGENTS_PROP ).toString();
        QList<QSharedPointer<GWSAgent> > agents_to_visit = GWSAgentEnvironment::globalInstance()->getByClass( "ContainerAgent" ) ;

        // Generate list of containers to visit:
        QList < GWSCoordinate > agents_to_visit_coord_array;
        QMap< QString , GWSCoordinate > agents_to_visit_coord_id_array;

        //QList<QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className()) ;
        foreach ( QSharedPointer<GWSAgent> a, agents_to_visit  ){
             GWSCoordinate agent_to_visit_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
             QString agent_to_visit_id = a->getProperty("@id").toString();
             agents_to_visit_coord_array.append( agent_to_visit_coord ) ;
             agents_to_visit_coord_id_array.insert( agent_to_visit_id , agent_to_visit_coord );
         }

        // Generate graph:
        const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );

        // Get graph edges:
        QList< QSharedPointer< GWSGraphEdge > > edges = graph->getEdges();

        // Generate TSP algorithm with those edges:
        GWSTSPRouting* agents_to_visit_route = new GWSTSPRouting( edges );

        // Get nearest neighbour given start coordinates and containers to visit:
        QList< GWSCoordinate > agents_to_visit_route_coord_array = agents_to_visit_route->nearestNeighborTsp( agent_home_coor , agents_to_visit_coord_array , agent_home_coor );

        // Order the nodes to get best route. This is the route to follow.
        QList< GWSCoordinate > ordered_agents_to_visit_tsp_route_coord_array = agents_to_visit_route->orderCircularTsp( agent_home_coor , agent_home_coor , agents_to_visit_route_coord_array );

        // Compare the coordinates of the ordered route and those in the initial QPair QList of coors and IDs to extract
        // the ID of the ordered coors and generate a new QList of QPairs:
        foreach (GWSCoordinate c, ordered_agents_to_visit_tsp_route_coord_array) {
            this->ordered_agents_to_visit_tsp_route_coord_id_array.append( qMakePair( c , agents_to_visit_coord_id_array.key( c ) ) );
        }
        return this->ordered_agents_to_visit_tsp_route_coord_id_array;
}
