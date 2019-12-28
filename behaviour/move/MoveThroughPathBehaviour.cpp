#include "MoveThroughPathBehaviour.h"

#include "../../util/geometry/Geometry.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../skill/move/MoveSkill.h"

QString geoworldsim::behaviour::MoveThroughPathBehaviour::INPUT_ROUTE_PATH = "input_route_path";

geoworldsim::behaviour::MoveThroughPathBehaviour::MoveThroughPathBehaviour() : MoveBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::MoveThroughPathBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();

    geometry::Geometry route_path = this->getProperty( INPUT_ROUTE_PATH ).toObject(); // Should be a LineString
    if( !route_path.isValid() ){
        qWarning() << QString("Entity %1 %2 has invalid route path to follow").arg( entity->metaObject()->className() ).arg( entity->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    QList< geometry::Coordinate > path_pending_coordinates = geometry::GeometryGetters::getCoordinates( route_path );

    // No more coordinates, arrived
    if( path_pending_coordinates.isEmpty() ){
        return QPair< double , QJsonArray >( 0 , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    // Set MoveBehaviour::INPUT_MOVE_TO property, for MoveBehaviour to behave
    geometry::Geometry next_path_point = geometry::Geometry( path_pending_coordinates.at( 0 ) );
    this->setProperty( MoveBehaviour::INPUT_MOVE_TO , next_path_point.getGeoJSON() );

    // Call parent method
    QPair<double, QJsonArray> result = MoveBehaviour::behave();

    // Once moved, check if reached next_path_point
    geometry::Geometry entity_geom_post = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Reached next_path_point
    if ( entity_geom_post.getDistance( next_path_point ) < unit::LengthUnit( 0.5 ) ){

        path_pending_coordinates.removeAt( 0 );

        // Finally reached path end
        if( path_pending_coordinates.isEmpty() ){

            return QPair< double , QJsonArray >( result.first , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );

        } else if ( path_pending_coordinates.size() == 1 ) { // Only one path_pending_coordinates left

            geometry::Coordinate c = path_pending_coordinates.at(0);
            this->setProperty( INPUT_ROUTE_PATH , QJsonObject({ { "type" , "Point" } , { "coordinates" , QJsonArray({ c.getX() , c.getY() , c.getZ() }) } }) );

            return QPair< double , QJsonArray >( result.first , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );

        } else { // Still have path_pending_coordinates, update route_path withouth the removed coor

            QJsonArray coors;
            foreach ( geometry::Coordinate c , path_pending_coordinates) {
                coors.append( QJsonArray({ c.getX() , c.getY() , c.getZ() }) );
            }
            this->setProperty( INPUT_ROUTE_PATH , QJsonObject({ { "type" , "LineString" } , { "coordinates" , coors } }) );

            return QPair< double , QJsonArray >( result.first , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
        }

    }

    // MoveBehaviour has not reached next_path_point
    return QPair< double , QJsonArray >( result.first ,  this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
}

