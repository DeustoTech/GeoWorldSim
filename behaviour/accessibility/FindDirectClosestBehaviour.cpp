#include "FindDirectClosestBehaviour.h"

#include "../../app/App.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"


QString FindDirectClosestBehaviour::ENTITY_TYPE_TO_FIND = "entity_type_to_find";  // e.g. glass Containerentity
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_ID_AS = "store_direct_closest_id_as";
QString FindDirectClosestBehaviour::STORE_DIRECT_CLOSEST_GEOM_AS = "store_direct_closest_geometry_as";
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

    QSharedPointer<GWSEntity> entity = this->getEntity();
     GWSGeometry entity_geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    GWSCoordinate entity_coor = entity_geom.getCentroid();

    QString facility_to_access = this->getProperty( ENTITY_TYPE_TO_FIND ).toString();
    QList<QSharedPointer<GWSEntity> > entitys_to_access = GWSEntityEnvironment::globalInstance()->getByClass( facility_to_access );

    // Obtain direct closest entity parameters:
    QSharedPointer<GWSEntity> nearest_entity = GWSPhysicalEnvironment::globalInstance()->getNearestEntity( entity_coor , entitys_to_access );
    if( !nearest_entity ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_DIRECT_CLOSEST_FOUND ).toArray() );
    }

    GWSGeometry nearest_entity_geom = nearest_entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
    GWSCoordinate nearest_entity_coor = nearest_entity_geom.getCentroid();
    GWSLengthUnit distanceToNearestentity = entity_coor.getDistance( nearest_entity_coor );

    // Store in entity:
    entity->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_ID_AS ).toString( "direct_closest_entity_uid" ) , nearest_entity->getUID() );
    entity->setProperty( this->getProperty( STORE_DIRECT_CLOSEST_GEOM_AS ).toString( "direct_closest_entity_geometry" ) , nearest_entity_geom.getGeoJSON() );
    entity->setProperty( this->getProperty( STORE_DIRECT_DISTANCE_AS ).toString( "direct_closest_entity_distance") , distanceToNearestentity.number() );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
