#include "StoreMultiRouteSkill.h"

QString GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_X_VALUE = "next_destination_x_value";
QString GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_Y_VALUE = "next_destination_y_value";
QString GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS = "next_agent_id";
QString GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS = "pending_route_destinations";

GWSStoreMultiRouteSkill::GWSStoreMultiRouteSkill(){

}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSStoreMultiRouteSkill::addDestination( QString agent_id , QJsonObject extra_properties ){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString( STORE_NEXT_DESTINATION_AGENT_ID_AS ) , agent_id );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}

void GWSStoreMultiRouteSkill::addDestination( GWSCoordinate coor , QJsonObject extra_properties ){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( PENDING_ROUTE_DESTINATIONS ).toArray();

    QJsonObject new_destination = extra_properties;
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION_X_VALUE ).toString( STORE_NEXT_DESTINATION_X_VALUE ) , coor.getX() );
    new_destination.insert( this->getProperty( STORE_NEXT_DESTINATION_Y_VALUE ).toString( STORE_NEXT_DESTINATION_Y_VALUE ) , coor.getY() );
    ordered_destinations.append( new_destination );

    agent->setProperty( PENDING_ROUTE_DESTINATIONS , ordered_destinations );
}
