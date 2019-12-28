#include "MoveThroughRouteInVehicleBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


MoveThroughRouteInVehicleBehaviour::MoveThroughRouteInVehicleBehaviour(): MoveThroughRouteBehaviour (){
}

MoveThroughRouteInVehicleBehaviour::~MoveThroughRouteInVehicleBehaviour(){
}

/**********************************************************************
 INITIALIZE
**********************************************************************/

void MoveThroughRouteInVehicleBehaviour::initialize(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteInVehicleSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteInVehicleSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteInVehicleSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteInVehicleSkill>( new MoveThroughRouteInVehicleSkill() );
        agent->addSkill( movethroughroute_skill );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/

/*QJsonArray MoveThroughRouteInVehicleBehaviour::behave(){

        QSharedPointer<GWSAgent> agent = this->getAgent();

        // Check if agent has a MoveThroughRouteInVehicleSkill, otherwise create it and set its max_speed
        QSharedPointer<MoveThroughRouteInVehicleSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteInVehicleSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteInVehicleSkill>();

        // Tick in 1 second duration to move in small parts
        GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

        QJsonValue x_destination = this->getProperty( BEHAVIOUR_DESTINATION_X_VALUE );
        QJsonValue y_destination = this->getProperty( BEHAVIOUR_DESTINATION_Y_VALUE );
        movethroughroute_skill->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_X_PROP , x_destination );
        movethroughroute_skill->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_Y_PROP , y_destination );

        GWSCoordinate destination_coor = movethroughroute_skill->getRouteDestination();
        if( !destination_coor.isValid() ){
            return this->getProperty( BEHAVIOUR_NEXTS_IF_NOT_ARRIVED ).toArray();
        }

        QSharedPointer<GWSGeometry> agent_geom_init = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
        GWSCoordinate agent_position_init = agent_geom_init->getCentroid();

        // Move towards
        movethroughroute_skill->setProperty( MoveThroughRouteSkill::SKILL_NETWORK_TYPE_PROP , this->getProperty( BEHAVIOUR_NETWORK_TYPE_PROP ) );

        movethroughroute_skill->move( duration_of_movement );
        QSharedPointer<GWSGeometry> agent_geom_post = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
        GWSCoordinate agent_position_post = agent_geom_post->getCentroid();

        // Set NEXTS behaviour
        if ( agent_position_post.getDistance( destination_coor ) < GWSLengthUnit(1) ){
           return this->getProperty( BEHAVIOUR_NEXTS_IF_ARRIVED ).toArray();
        }

        if ( agent_position_post != destination_coor ){
           return this->getProperty( BEHAVIOUR_NEXTS_IF_NOT_ARRIVED ).toArray();
        }


}*/
