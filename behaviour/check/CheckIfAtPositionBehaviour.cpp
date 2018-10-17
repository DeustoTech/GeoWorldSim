#include "CheckIfAtPositionBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"

QString CheckIfAtPositionBehaviour::X_VALUE = "fixed_x";
QString CheckIfAtPositionBehaviour::Y_VALUE = "fixed_y";
QString CheckIfAtPositionBehaviour::X_VALUE_FROM_AGENT_PROPERTY = "x_from_property";
QString CheckIfAtPositionBehaviour::Y_VALUE_FROM_AGENT_PROPERTY = "y_from_property";
QString CheckIfAtPositionBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckIfAtPositionBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CheckIfAtPositionBehaviour::CheckIfAtPositionBehaviour() : GWSBehaviour(){
}


QStringList CheckIfAtPositionBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
    GWSCoordinate position_to_check;

    if ( !this->getProperty( X_VALUE ).isNull() && !this->getProperty( Y_VALUE ).isNull() ){

        double coor_x = this->getProperty( X_VALUE ).toDouble();
        double coor_y = this->getProperty( Y_VALUE ).toDouble();

        position_to_check = GWSCoordinate( coor_x , coor_y );


    }

    if ( !this->getProperty( X_VALUE_FROM_AGENT_PROPERTY ).isNull() && !this->getProperty( X_VALUE_FROM_AGENT_PROPERTY ).isNull() ){

        double coor_x = this->getProperty( X_VALUE_FROM_AGENT_PROPERTY ).toDouble();
        double coor_y = this->getProperty( X_VALUE_FROM_AGENT_PROPERTY ).toDouble();

        position_to_check = GWSCoordinate( coor_x , coor_y );

    }

    QStringList nexts;

    if ( agent_coord == position_to_check ){
        nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();

    }
    else {
        nexts = this->getProperty( NEXTS_IF_FALSE ).toStringList();
    }

    return nexts;
}
