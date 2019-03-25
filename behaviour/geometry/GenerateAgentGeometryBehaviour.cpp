#include "GenerateAgentGeometryBehaviour.h"
#include "../../util/geometry/Geometry.h"
#include "../../app/App.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString GenerateAgentGeometryBehaviour::NEXTS = "nexts";
QString GenerateAgentGeometryBehaviour::SET_X_VALUE = "x_value";
QString GenerateAgentGeometryBehaviour::SET_Y_VALUE = "y_value";
QString GenerateAgentGeometryBehaviour::SET_JSON_VALUE = "json_value";

GenerateAgentGeometryBehaviour::GenerateAgentGeometryBehaviour() : GWSBehaviour(){

}

QJsonArray GenerateAgentGeometryBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSNewGeometry agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

   if( agent_geom.isValid() ){
        GWSPhysicalEnvironment::globalInstance()->unregisterAgent( agent );
    }

    QJsonValue x_value = this->getProperty( SET_X_VALUE );
    QJsonValue y_value = this->getProperty( SET_Y_VALUE );
    if( !x_value.isNull() && !y_value.isNull() ){

        GWSCoordinate destination_coor = GWSCoordinate( x_value.toDouble() , y_value.toDouble() );
        QJsonObject geom_json;
        geom_json.insert( "type" , "Point" );
        QJsonArray coordinates;
        coordinates.append( destination_coor.getX() ); coordinates.append( destination_coor.getY() );
        geom_json.insert( "coordinates" , coordinates );
        geom_json.insert( GWSObject::GWS_CLASS_PROP , GWSGeometry::staticMetaObject.className() );
        agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , geom_json );
        GWSPhysicalEnvironment::globalInstance()->registerAgent( agent );
    }

    QJsonValue json_value = this->getProperty( SET_JSON_VALUE );
    if( !json_value.isNull() ){

        QJsonObject geom_json = json_value.toObject();
        agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , geom_json );
        GWSPhysicalEnvironment::globalInstance()->registerAgent( agent );

    }

   return this->getProperty( NEXTS ).toArray();
}
