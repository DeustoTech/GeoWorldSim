#include "FindDirectClosestBehaviour.h"

#include "../../app/App.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"


QString FindDirectClosestBehaviour::AGENT_TO_ACCESS_TYPE = "facility_type";  // e.g. glass ContainerAgent
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_ID_AS = "store_direct_closest_id_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_X_AS = "store_direct_closest_x_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_Y_AS = "store_direct_closest_y_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_DISTANCE_AS = "store_direct_distance_as";
QString FindDirectClosestBehaviour::NEXTS = "nexts";
QString FindDirectClosestBehaviour::NEXTS_IF_NO_DIRECT_CLOSEST_FOUND = "nexts_if_no_direct_closest_found";



FindDirectClosestBehaviour::FindDirectClosestBehaviour() : GWSBehaviour(){
}

FindDirectClosestBehaviour::~FindDirectClosestBehaviour(){
}


/************************************************************
 * SLOTS
 ***********************************************************/
QJsonArray FindDirectClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    QString facility_to_access = this->getProperty( AGENT_TO_ACCESS_TYPE ).toString();
    QList<QSharedPointer<GWSAgent> > agents_to_access = GWSAgentEnvironment::globalInstance()->getByClass( facility_to_access );

    // Obtain direct closest agent parameters:
    QSharedPointer<GWSAgent> nearest_agent = GWSPhysicalEnvironment::globalInstance()->getNearestAgent( agent_coor , agents_to_access );
    if( !nearest_agent ){
        return this->getProperty( NEXTS_IF_NO_DIRECT_CLOSEST_FOUND ).toArray();
    }

    GWSCoordinate nearest_agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( nearest_agent->getUID() )->getCentroid();
    GWSLengthUnit distanceToNearestAgent = agent_coor.getDistance( nearest_agent_coor );

    // Store in agent:
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_ID_AS ).toString( "direct_closest_agent_id" ) , nearest_agent->getUID() );
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_X_AS ).toString( "direct_closest_agent_x" ) , nearest_agent_coor.getX() );
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_Y_AS ).toString( "direct_closest_agent_y" ) , nearest_agent_coor.getY() );
    agent->setProperty( this->getProperty( STORE_DIRECT_DISTANCE_AS ).toString("direct_closest_agent_distance") , distanceToNearestAgent.number() );

    return this->getProperty( NEXTS ).toArray();
}
