#include <QFuture>

#include "CalculateTSPRouteBehaviour.h"
#include "../../skill/move/GenerateOrderedTSPSkill.h"

QString CalculateTSPRouteBehaviour::NEXTS = "nexts";
QString CalculateTSPRouteBehaviour::TSP_AGENT_TYPE = "tsp_agent_type"; // e.g. Glass Container
QString CalculateTSPRouteBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";  // e.g. roads
QString CalculateTSPRouteBehaviour::STORE_TSP_ROUTE_AS = "store_tsp_route_as";

CalculateTSPRouteBehaviour::CalculateTSPRouteBehaviour() : GWSBehaviour(){

}

/*
 * Generate Ordered TSP route with Agent Type input
 */

QStringList  CalculateTSPRouteBehaviour::behave(){

        QSharedPointer<GWSAgent> agent = this->getAgent();
        GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();

        if ( agent->getProperty( this->getProperty( STORE_TSP_ROUTE_AS ).toString() ).isNull() ){

             GWSCoordinate agent_position = env->getGeometry( agent )->getCentroid();
             QString agent_type = this->getProperty( TSP_AGENT_TYPE ).toString();

             // Get agent class to visit depending on user input:
            QList<QSharedPointer<GWSAgent> > agents_to_visit = GWSAgentEnvironment::globalInstance()->getByClass( agent_type ) ;

             // Generate list of containers to visit:
             QList < GWSCoordinate > agents_to_visit_coord_array;
             QStringList agents_to_visit_id_array;
             QMap< QString , GWSCoordinate > agents_to_visit_coord_id_array;

             foreach ( QSharedPointer<GWSAgent> a, agents_to_visit  ){
                  GWSCoordinate agent_to_visit_coord = env->getGeometry( a )->getCentroid();
                  QString agent_to_visit_id = a->getProperty("@id").toString();
                  agents_to_visit_coord_array.append( agent_to_visit_coord ) ;
                  agents_to_visit_id_array.append( agent_to_visit_id );
                  agents_to_visit_coord_id_array.insert( agent_to_visit_id , agent_to_visit_coord );

              }

             // Generate graph:
             const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );

             // Get graph edges:
             QList< QSharedPointer< GWSGraphEdge > > edges = graph->getEdges();

             // Generate TSP algorithm with those edges:
             GWSTSPRouting* agents_to_visit_route = new GWSTSPRouting( edges );
             agents_to_visit_route->moveToThread( this->thread() );

             // Get nearest neighbour given start coordinates and containers to visit:
             QList< GWSCoordinate > agents_to_visit_route_coord_array = agents_to_visit_route->nearestNeighborTsp( agent_position , agents_to_visit_coord_array , agent_position );

             // Order the nodes to get best route. This is the route to follow.
             QList< GWSCoordinate > ordered_agents_to_visit_tsp_route_coord_array = agents_to_visit_route->orderCircularTsp( agent_position , agent_position , agents_to_visit_route_coord_array );

             // Compare the coordinates of the ordered route and those in the initial QPair QList of coors and IDs to extract
             // the ID of the ordered coors and generate a new QList of QPairs:
             foreach (GWSCoordinate c, ordered_agents_to_visit_tsp_route_coord_array) {
                 this->ordered_agents_to_visit_tsp_route_coord_id_array.append( qMakePair( c , agents_to_visit_coord_id_array.key( c ) ) );
             }

             // Extract array of ordered nodes IDs:
             QPair<GWSCoordinate, QString> pair;
             foreach ( pair , this->ordered_agents_to_visit_tsp_route_coord_id_array ){
                 QString id = pair.second;
                 this->ordered_agents_to_visit_tsp_route_id_array.append( id );
             }

             QStringList route_ids = this->ordered_agents_to_visit_tsp_route_id_array;
             agent->setProperty( this->getProperty( STORE_TSP_ROUTE_AS ).toString() , route_ids ) ;

        }


        QStringList nexts = this->getProperty( NEXTS ).toStringList();
        return nexts;
}



