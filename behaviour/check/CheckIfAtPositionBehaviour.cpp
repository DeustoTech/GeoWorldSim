#include "CheckIfAtPositionBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"


QString CheckIfAtPositionBehaviour::PROPERTY_NAME_X = "property_name_x";
QString CheckIfAtPositionBehaviour::PROPERTY_NAME_Y = "property_name_y";
QString CheckIfAtPositionBehaviour::NEXTS_IF_AT_POSITION = "nexts_if_at_position";
QString CheckIfAtPositionBehaviour::NEXTS_IF_NOT_AT_POSITION = "nexts_if_not_at_position";



CheckIfAtPositionBehaviour::CheckIfAtPositionBehaviour() : GWSBehaviour(){
}


QStringList CheckIfAtPositionBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    double coor_x = this->getProperty( PROPERTY_NAME_X ).toDouble();
    double coor_y = this->getProperty( PROPERTY_NAME_Y ).toDouble();

    GWSCoordinate key_position = GWSCoordinate( coor_x , coor_y );

    if (agent_coord == key_position ){

        QStringList next = this->getProperty( NEXTS_IF_AT_POSITION );
        return next;
    }
    else {

        QStringList next = this->getProperty( NEXTS_IF_NOT_AT_POSITION );
        return next;
    }

}
