#include "GenerateAgentGeometryBehaviour.h"
#include "../../util/geometry/Geometry.h"
#include "../../app/App.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString GenerateAgentGeometryBehaviour::NEXTS = "nexts";
QString GenerateAgentGeometryBehaviour::X_VALUE = "x_value";
QString GenerateAgentGeometryBehaviour::Y_VALUE = "y_value";


GenerateAgentGeometryBehaviour::GenerateAgentGeometryBehaviour() : GWSBehaviour(){

}

QJsonArray GenerateAgentGeometryBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    QJsonValue x_value = this->getProperty( X_VALUE );
    QJsonValue y_value = this->getProperty( Y_VALUE );

    GWSCoordinate destination_coor = GWSCoordinate( x_value.toString().toDouble() , y_value.toString().toDouble() );

    if( agent_geom.isNull() ){

        QJsonObject geom_json;
        geom_json.insert( GWSObject::GWS_TYPE_PROP , GWSGeometry::staticMetaObject.className() );
        geom_json.insert( "type" , "Point" );
        QJsonArray coordinates;
        coordinates.append( destination_coor.getX() ); coordinates.append( destination_coor.getY() );
        geom_json.insert( "coordinates" , coordinates );
        agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , geom_json );
        GWSPhysicalEnvironment::globalInstance()->registerAgent( agent );

    } else {

        // Set the geometry of the agent through the TransferMove method within the physical environment:
        GWSCoordinate current_coor = agent_geom->getCentroid();


        // Displacement
        double x_distance = destination_coor.getX() - current_coor.getX() ;
        double y_distance = destination_coor.getY() - current_coor.getY() ;

        // Set the agents position
        GWSCoordinate apply_movement = GWSCoordinate( x_distance , y_distance );
        GWSPhysicalEnvironment::globalInstance()->transformMove( agent , apply_movement );

        //qDebug() << new_agent_geom->getCentroid().toString();

    }
    return this->getProperty( NEXTS ).toArray();
}
