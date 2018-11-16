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

    QString new_agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().toString();

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;

}
