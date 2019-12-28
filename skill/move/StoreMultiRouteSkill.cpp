#include "StoreMultiRouteSkill.h"

QString GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION = "next_destination";
QString GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS = "next_agent_id";
QString GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS = "pending_route_destinations";

GWSStoreMultiRouteSkill::GWSStoreMultiRouteSkill(){

}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSStoreMultiRouteSkill::addDestination( const QString &agent_id , const QJsonObject& extra_properties ){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString( STORE_NEXT_DESTINATION_AGENT_ID_AS ) , agent_id );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}


void GWSStoreMultiRouteSkill::addDestination( const GWSGeometry &geom , const QJsonObject& extra_properties ){
    this->addDestination( geom.getGeoJSON() , extra_properties );
}

void GWSStoreMultiRouteSkill::addDestination( const GWSCoordinate &coor , const QJsonObject& extra_properties ){
    this->addDestination( GWSGeometry( coor ).getGeoJSON() , extra_properties );
}

void GWSStoreMultiRouteSkill::addDestination(const QJsonObject &geojson, const QJsonObject &extra_properties){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION ).toString( STORE_NEXT_DESTINATION ) , geojson );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}
