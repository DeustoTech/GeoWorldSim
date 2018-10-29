#include "GenerateAgentGeometryBehaviour.h"
#include "../../util/geometry/Geometry.h"
#include "../../app/App.h"


#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString GenerateAgentGeometryBehaviour::NEXTS = "nexts";
QString GenerateAgentGeometryBehaviour::X_VALUE = "x_value";
QString GenerateAgentGeometryBehaviour::Y_VALUE = "y_value";


GenerateAgentGeometryBehaviour::GenerateAgentGeometryBehaviour() : GWSBehaviour(){

}

QStringList GenerateAgentGeometryBehaviour::behave(){
    bool ok = false;
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    QVariant x_value = this->getProperty( X_VALUE );
    QVariant y_value = this->getProperty( Y_VALUE );

    bool x_is_property = x_value.toString().startsWith( "<" ) && x_value.toString().endsWith( ">" );
    bool y_is_property = y_value.toString().startsWith( "<" ) && y_value.toString().endsWith( ">" );


    if ( x_is_property && y_is_property ){

        QString x_property_name = x_value.toString().remove( 0 , 1 );
        x_property_name = x_property_name.remove( x_property_name.length() - 1 , 1 );
        x_value = agent->getProperty( x_property_name );

        QString y_property_name = y_value.toString().remove( 0 , 1 );
        y_property_name = y_property_name.remove( y_property_name.length() - 1 , 1 );
        y_value = agent->getProperty( y_property_name );
    }

    GWSCoordinate destination_coor = GWSCoordinate( x_value.toDouble(&ok) , y_value.toDouble(&ok) );

    if( agent_geom.isNull() ){

        GWSPhysicalEnvironment::globalInstance()->transformMove( agent , destination_coor );

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

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;



}
