#include "CalculateTSPRouteBehaviour.h"
#include "../../skill/move/GenerateOrderedTSPSkill.h"

QString CalculateTSPRouteBehaviour::NEXTS = "nexts";
QString CalculateTSPRouteBehaviour::TSP_AGENT_TYPE = "tsp_agent_type"; // e.g. Glass Container
QString CalculateTSPRouteBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";  // e.g. roads
QString CalculateTSPRouteBehaviour::STORE_TSP_ROUTE_AS_PROPERTY_NAME = "store_tsp_route_as";
//QString CalculateTSPRouteBehaviour::STORE_CLOSEST_ID_AS = "store_closest_id_as";
//QString CalculateTSPRouteBehaviour::TSP_ROUTE_STAGE = "tsp_route_stage";
//QString CalculateTSPRouteBehaviour::STORE_TSP_ROUTE_STAGE_X_AS = "store_tsp_route_stage_x_as";
//QString CalculateTSPRouteBehaviour::STORE_TSP_ROUTE_STAGE_Y_AS = "store_tsp_route_stage_y_as";

CalculateTSPRouteBehaviour::CalculateTSPRouteBehaviour() : GWSBehaviour(){

}

/*
 * Generate Ordered TSP route with Agent Type input
 */

QJsonArray CalculateTSPRouteBehaviour::behave(){

        QSharedPointer<GWSAgent> agent = this->getAgent();
        GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();

        QSharedPointer<GWSGeometry> agent_geom = env->getGeometry( agent );
        GWSCoordinate agent_position = agent_geom->getCentroid();

        QString agent_type = this->getProperty( TSP_AGENT_TYPE ).toString();

         // Get agent class to visit depending on user input:
        QList< QSharedPointer<GWSAgent> > agents_to_visit = GWSAgentEnvironment::globalInstance()->getByClass( agent_type ) ;

         // Generate list of containers to visit:
         QList<GWSCoordinate> agents_to_visit_coords;
         QStringList agents_to_visit_id_array;
         QMap< QString , GWSCoordinate > agents_to_visit_coord_id_array;

         foreach ( QSharedPointer<GWSAgent> a , agents_to_visit  ){

              QSharedPointer<GWSGeometry> a_geom = env->getGeometry( a );
              GWSCoordinate agent_to_visit_coord = a_geom->getCentroid();

              QString agent_to_visit_id = a->getProperty("@id").toString();
              agents_to_visit_coords.append( agent_to_visit_coord ) ;
              agents_to_visit_id_array.append( agent_to_visit_id );
              agents_to_visit_coord_id_array.insert( agent_to_visit_id , agent_to_visit_coord );

          }

         // Generate TSP algorithm with those edges
         GWSTSPRouting* routing = new GWSTSPRouting( this->getProperty( TRANSPORT_NETWORK_TYPE ).toString() );

         // Get nearest neighbour given start coordinates and containers to visit
         QList< GWSCoordinate > agents_to_visit_route_coord_array = routing->nearestNeighborTsp( agent_position , agents_to_visit_coords , agent_position );

         // Order the nodes to get best route. This is the route to follow.
         QList< GWSCoordinate > ordered_agents_to_visit_tsp_route_coord_array = routing->orderCircularTsp( agent_position , agent_position , agents_to_visit_route_coord_array );

         // Compare the coordinates of the ordered route and those in the initial QPair QList of coors and IDs to extract
         // the ID of the ordered coors and generate a new QList of QPairs:
         QList< QPair< GWSCoordinate , QString > > ordered_agents_to_visit_tsp_route_coord_id_array;
         QJsonArray ordered_agents_to_visit_tsp_route_id_array;

         foreach (GWSCoordinate c, ordered_agents_to_visit_tsp_route_coord_array) {
             ordered_agents_to_visit_tsp_route_coord_id_array.append( qMakePair( c , agents_to_visit_coord_id_array.key( c ) ) );
         }

         // Extract array of ordered nodes IDs:
         QPair<GWSCoordinate, QString> pair;
         foreach ( pair , ordered_agents_to_visit_tsp_route_coord_id_array ){
             QString id = pair.second;
             ordered_agents_to_visit_tsp_route_id_array.append( id );
         }

         agent->setProperty( this->getProperty( STORE_TSP_ROUTE_AS_PROPERTY_NAME ).toString() , ordered_agents_to_visit_tsp_route_id_array );
         //agent->setProperty( TSP_ROUTE_STAGE , QJsonValue() );
         routing->deleteLater();
         return this->getProperty( NEXTS ).toArray();

       /* else {


            int current_tsp_route_stage = agent->getProperty( TSP_ROUTE_STAGE ).toInt();
            tsp_route_ids = this->ordered_agents_to_visit_tsp_route_id_array;
            int tsp_route_size = tsp_route_ids.size();
            QJsonValue current_tsp_route_stage_id = tsp_route_ids.at( current_tsp_route_stage );

            if ( current_tsp_route_stage <= ( tsp_route_size - 1 )  ){

               QSharedPointer< GWSAgent > current_tsp_stage_agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( "ContainerAgent" ,  current_tsp_route_stage_id.toString() );
               GWSCoordinate current_tsp_stage_agent_coors = GWSPhysicalEnvironment::globalInstance()->getGeometry( current_tsp_stage_agent )->getCentroid();

               agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_X_AS ).toString() , current_tsp_stage_agent_coors.getX() );
               agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_Y_AS ).toString() , current_tsp_stage_agent_coors.getY() );
               agent->setProperty( this->getProperty( STORE_CLOSEST_ID_AS ).toString() , current_tsp_stage_agent->getId() );
               agent->setProperty( TSP_ROUTE_STAGE , current_tsp_route_stage + 1 );

               }
              else {
               agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_X_AS ).toString() , agent->getProperty( "home_x" ).toDouble() );
               agent->setProperty( this->getProperty( STORE_TSP_ROUTE_STAGE_Y_AS ).toString() , agent->getProperty( "home_y" ).toDouble() );
               agent->setProperty( TSP_ROUTE_STAGE , QJsonValue() );
            }

            return this->getProperty( NEXTS_IF_TSP ).toArray();

        }*/
}



