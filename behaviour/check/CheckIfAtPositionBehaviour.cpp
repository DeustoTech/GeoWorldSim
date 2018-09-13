#include "CheckIfAtPositionBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString CheckIfAtPositionBehaviour::KEY_POSITION_X = "key_position_x";
QString CheckIfAtPositionBehaviour::KEY_POSITION_Y = "key_position_y";


CheckIfAtPositionBehaviour::CheckIfAtPositionBehaviour() : GWSBehaviour(){
}


bool CheckIfAtPositionBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();


    //qDebug() << this->getProperty( KEY_POSITION_X );
    //double home_coordX = agent->getProperty("home_coordX").toDouble();
    double key_x = this->getProperty( KEY_POSITION_X ).toDouble();
    double key_y = this->getProperty( KEY_POSITION_Y ).toDouble();

    //qDebug() << key_x;

    GWSCoordinate key_position = GWSCoordinate( key_x , key_y );

    if (agent_coord == key_position ){
        qDebug() << "At home!";
        return true;
    }

    qDebug() << "Not at home!";
    return false;
}


bool CheckIfAtPositionBehaviour::behave(){
     // DO NOTHING IF NOT AT POSITION
    return true;
}
