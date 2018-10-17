#include "CheckIfAtPositionBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"

QString CheckIfAtPositionBehaviour::X_VALUE = "position_x";
QString CheckIfAtPositionBehaviour::Y_VALUE = "position_y";
QString CheckIfAtPositionBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString CheckIfAtPositionBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


CheckIfAtPositionBehaviour::CheckIfAtPositionBehaviour() : GWSBehaviour(){
}


QStringList CheckIfAtPositionBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    QVariant x_position = this->getProperty( X_VALUE );
    QVariant y_position = this->getProperty( Y_VALUE );

    bool x_is_property = x_position.toString().startsWith( "<" ) && x_position.toString().endsWith( ">" );
    bool y_is_property = y_position.toString().startsWith( "<" ) && y_position.toString().endsWith( ">" );

    if ( x_is_property && y_is_property ){

        QString x_property_name = x_position.toString().remove( 0 , 1 );
        QString y_property_name = y_position.toString().remove( 0 , 1 );

        x_property_name = x_property_name.remove( x_property_name.length() - 1 , 1 );
        y_property_name = y_property_name.remove( y_property_name.length() - 1 , 1 );

        x_position = agent->getProperty( x_property_name );
        y_position = agent->getProperty( y_property_name );

    }

    if ( x_is_property && !y_is_property || !x_is_property && y_is_property ){
        qWarning() << "Watch out! Wrong format for position property!";
    }

    GWSCoordinate position_to_check = GWSCoordinate( x_position.toDouble() , y_position.toDouble() );
    QStringList nexts;

    if ( agent_coord == position_to_check ){
        nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();

    }
    else {
        nexts = this->getProperty( NEXTS_IF_FALSE ).toStringList();
    }

    return nexts;
}
