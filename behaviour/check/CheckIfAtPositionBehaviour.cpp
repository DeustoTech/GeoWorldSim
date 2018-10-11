#include "CheckIfAtPositionBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"

QString CheckIfAtPositionBehaviour::POSITION_X_VALUE = "position_x_value";
QString CheckIfAtPositionBehaviour::POSITION_Y_VALUE = "position_y_value";
QString CheckIfAtPositionBehaviour::NEXTS_IF_AT_POSITION = "nexts_if_at_position";
QString CheckIfAtPositionBehaviour::NEXTS_IF_NOT_AT_POSITION = "nexts_if_not_at_position";


CheckIfAtPositionBehaviour::CheckIfAtPositionBehaviour() : GWSBehaviour(){
}


QStringList CheckIfAtPositionBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    double coor_x = this->getProperty( POSITION_X_VALUE ).toDouble();
    double coor_y = this->getProperty( POSITION_Y_VALUE ).toDouble();

    GWSCoordinate check_position = GWSCoordinate( coor_x , coor_y );

    if (agent_coord == check_position ){

        return this->getProperty( NEXTS_IF_AT_POSITION ).toStringList();
    } else {

        return this->getProperty( NEXTS_IF_NOT_AT_POSITION ).toStringList();
    }

}
