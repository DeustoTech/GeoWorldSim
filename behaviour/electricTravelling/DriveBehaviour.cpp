#include "DriveBehaviour.h"
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/drive/DriveSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString DriveBehaviour::VEHICLE_TYPE = "vehicle_type";
QString DriveBehaviour::VEHICLE_SUBTYPE = "vehicle_subtype";
QString DriveBehaviour::VEHICLE_LOAD = "vehicle_load";
QString DriveBehaviour::SPEED_FACTOR_PROP = "speed_factor";
QString DriveBehaviour::SECURITY_DISTANCE = "security_distance";
QString DriveBehaviour::NEXTS_IF_MOVE = "nexts_if_move";
QString DriveBehaviour::NEXTS_IF_STOP = "nexts_if_stop";


DriveBehaviour::DriveBehaviour() : GWSBehaviour(){

}

DriveBehaviour::~DriveBehaviour(){
}


QStringList DriveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    QStringList nexts_if_stopped;
    QStringList nexts_if_moving = this->getProperty( NEXTS_IF_MOVE ).toStringList();

    Q_ASSERT( !agent_geom.isNull() );

    // ------------------------
    // Get current road ID
    // ------------------------

    QString current_road_id = agent->getProperty( MoveThroughRouteSkill::AGENT_CURRENT_ROAD_ID_PROP ).toString();

     // ------------------------------------
     // Cannot decide speed without info about road
     //-------------------------------------

     if ( current_road_id.isEmpty() ){
         // Move at slow speed
         agent->setProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP , 40 );
         return nexts_if_moving;
     }

     // ------------------------------------
     // Check if already moving towards direction
     //-------------------------------------

     // Check if agent has a MoveSkill, otherwise create it and set its max_speed
     QSharedPointer<MoveSkill> move_skill = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
     if( move_skill.isNull() ){
         move_skill = QSharedPointer<MoveSkill>( new MoveSkill() );
         agent->addSkill( move_skill );
     }
     GWSCoordinate direction = move_skill->getMovingTowardsCoordinate();

    // -------------------------
    // Get current road maxSpeed
    // -------------------------

    GWSSpeedUnit road_maxSpeed = agent->getProperty( MoveThroughRouteSkill::AGENT_CURRENT_ROAD_MAXSPEED_PROP ).toDouble();

    // ---------------------------------------
    // Get vehicle speed
    // ---------------------------------------

    GWSSpeedUnit speed = agent->getProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP ).toDouble();

    // ---------------------------------------
    // Get road occupation and nearest vehicle
    // ---------------------------------------

    // Get agent from road ID:
    QSharedPointer<GWSAgent> current_edge_agent = GWSAgentEnvironment::globalInstance()->getById( current_road_id );
    QStringList agents_id_inside_current_edge = current_edge_agent->getProperty( GWSNetworkEnvironment::INSIDE_AGENT_IDS_PROP ).toStringList();
    QList< QSharedPointer<GWSAgent> > agents_inside_current_edge = GWSAgentEnvironment::globalInstance()->getByIds( agents_id_inside_current_edge );
    QSharedPointer<GWSAgent> agent_in_front_of_me = Q_NULLPTR;
    GWSLengthUnit agent_in_fron_of_me_distance;

    foreach ( QSharedPointer<GWSAgent> a , agents_inside_current_edge ){
        if ( a->getId() == agent->getId() ){ // Fill list with agents other than itself
            continue;
        }
        QSharedPointer<GWSGeometry> a_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( a );
        if( !a_geom ){
            continue;
        }

        if( a_geom->getCentroid().getDistance( direction ) >= agent_geom->getCentroid().getDistance( direction ) ){
            continue;
        }

        GWSLengthUnit distance = a_geom->getDistance( agent_geom );

        // Init agent and distance
        if( !agent_in_front_of_me ){
            agent_in_front_of_me = a;
            agent_in_fron_of_me_distance = distance;
        }

        if( distance < agent_in_fron_of_me_distance ){
            agent_in_front_of_me = a;
        }
    }

  if ( !agent_in_front_of_me ){ // No other agent apart from the agent itself

         agent->setProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP , road_maxSpeed );
         agent->setProperty( "color" , "Blue" );

    } else {

        GWSLengthUnit security_distance = this->getProperty( SECURITY_DISTANCE ).toDouble();

        if ( agent_in_fron_of_me_distance > security_distance ){

            agent->setProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP , road_maxSpeed );
            agent->setProperty( "color" , "Blue" );

        } else {

            agent->setProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP , GWSSpeedUnit( 0 ) );
            agent->setProperty( "color" , "Red" );
            return this->getProperty( NEXTS_IF_STOP ).toStringList();
        }
    }

    return nexts_if_moving;
}
