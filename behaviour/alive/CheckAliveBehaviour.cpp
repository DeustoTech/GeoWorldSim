#include "CheckAliveBehaviour.h"

/*  This behaviour aims at checking whether a GWSAgent has energy > 1 */

CheckAliveBehaviour::CheckAliveBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}

bool CheckAliveBehaviour::finished(){

    if (this->getAgent()->getProperty("energy") >= 1.)
        {
        qDebug() << this->getAgent()->getProperty("@id").toString() << " you are alive, keep going!";
        qDebug() << this->getAgent()->getProperty("energy").toString() << " energy!";
        return true;
        }
    else
        {
        this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
        qInfo() << "RIP" << this->getAgent()->getProperty("@id").toString();
        return false;
        }

}
