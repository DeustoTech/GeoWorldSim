#include "FindDirectClosestBehaviour.h"

#include "../../app/App.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"


QString FindDirectClosestBehaviour::ENTITY_TO_ACCESS_TYPE = "facility_type";  // e.g. glass ContainerAgent
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_ID_AS = "store_direct_closest_id_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_X_AS = "store_direct_closest_x_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_Y_AS = "store_direct_closest_y_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_GEOM_AS = "store_direct_closest_geom_as";
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
QPair< double , QJsonArray > FindDirectClosestBehaviour::behave(){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent ).getCentroid();

    QString facility_to_access = this->getProperty( ENTITY_TO_ACCESS_TYPE ).toString();
    QList<QSharedPointer<GWSEntity> > agents_to_access = GWSEntityEnvironment::globalInstance()->getByClass( facility_to_access );

    // Obtain direct closest agent parameters:
    QSharedPointer<GWSEntity> nearest_agent = GWSPhysicalEnvironment::globalInstance()->getNearestAgent( agent_coor , agents_to_access );
    if( !nearest_agent ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_DIRECT_CLOSEST_FOUND ).toArray() );
    }

    GWSGeometry nearest_agent_geom = nearest_agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
    GWSCoordinate nearest_agent_coor = nearest_agent_geom.getCentroid();
    GWSLengthUnit distanceToNearestAgent = agent_coor.getDistance( nearest_agent_coor );

    // Store in agent:
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_ID_AS ).toString( "direct_closest_agent_id" ) , nearest_agent->getUID() );
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_X_AS ).toString( "direct_closest_agent_x" ) , nearest_agent_coor.getX() );
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_Y_AS ).toString( "direct_closest_agent_y" ) , nearest_agent_coor.getY() );
    agent->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_GEOM_AS ).toString( "direct_closest_agent_geom" ) , nearest_agent_geom.getGeoJSON() );
    agent->setProperty( this->getProperty( STORE_DIRECT_DISTANCE_AS ).toString( "direct_closest_agent_distance") , distanceToNearestAgent.number() );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
