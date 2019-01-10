#include "MoveThroughRouteInVehicleBehaviour.h"


MoveThroughRouteInVehicleBehaviour::MoveThroughRouteInVehicleBehaviour(): GWSBehaviour(){
}

MoveThroughRouteInVehicleBehaviour::~MoveThroughRouteInVehicleBehaviour(){
}


QJsonArray MoveThroughRouteInVehicleBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a MoveThroughRouteInVehicleSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteInVehicleSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteInVehicleSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteInVehicleSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteInVehicleSkill>( new MoveThroughRouteInVehicleSkill() );
        agent->addSkill( movethroughroute_skill );
    }


    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;


   movethroughroute_skill->moveAndPollute( duration_of_movement );

}
