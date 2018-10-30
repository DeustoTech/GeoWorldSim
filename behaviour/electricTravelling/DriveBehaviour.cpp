#include "DriveBehaviour.h"
#include "../../skill/move/drive/DriveSkill.h"

QString DriveBehaviour::SPEED_FACTOR_PROP = "speed_factor";
QString DriveBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString DriveBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


DriveBehaviour::DriveBehaviour() : GWSBehaviour()
{

}


QStringList DriveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a DriveSkill, otherwise create it and set its max_speed
    QSharedPointer<DriveSkill> drive_skill = agent->getSkill( DriveSkill::staticMetaObject.className() ).dynamicCast<DriveSkill>();
    if( drive_skill.isNull() ){
        drive_skill = QSharedPointer<DriveSkill>( new DriveSkill() );
        agent->addSkill( drive_skill );
    }
    drive_skill->setProperty( DriveSkill::SPEED_FACTOR_PROP , this->getProperty( DriveBehaviour::SPEED_FACTOR_PROP ) );




}
