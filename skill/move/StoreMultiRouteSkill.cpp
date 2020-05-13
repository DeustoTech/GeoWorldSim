#include "StoreMultiRouteSkill.h"

QString geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION = "next_destination";
QString geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS = "next_agent_id";
QString geoworldsim::skill::StoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS = "pending_route_destinations";

geoworldsim::skill::StoreMultiRouteSkill::StoreMultiRouteSkill(){

}

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::skill::StoreMultiRouteSkill::addDestination( const QString &agent_id , const QJsonObject& extra_properties ){

    QSharedPointer<Entity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString( STORE_NEXT_DESTINATION_AGENT_ID_AS ) , agent_id );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}


void geoworldsim::skill::StoreMultiRouteSkill::addDestination( const geometry::Geometry &geom , const QJsonObject& extra_properties ){
    this->addDestination( geom.getGeoJSON() , extra_properties );
}

void geoworldsim::skill::StoreMultiRouteSkill::addDestination( const geometry::Coordinate &coor , const QJsonObject& extra_properties ){
    this->addDestination( geometry::Geometry( coor ).getGeoJSON() , extra_properties );
}

void geoworldsim::skill::StoreMultiRouteSkill::addDestination(const QJsonObject &geojson, const QJsonObject &extra_properties){

    QSharedPointer<Entity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION ).toString( STORE_NEXT_DESTINATION ) , geojson );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}
