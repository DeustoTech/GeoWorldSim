#include "CheckAliveBehaviour.h"

/*  This behaviour aims at checking whether a GWSAgent has energy > 1 */
#include "../../environment/execution_environment/ExecutionEnvironment.h"

CheckAliveBehaviour::CheckAliveBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}

bool CheckAliveBehaviour::finished(){

    if (this->getAgent()->getProperty("energy") > 0.)
        {
        qDebug() << this->getAgent()->getProperty("@id").toString() << " you are alive, keep going!";
        qDebug() << this->getAgent()->getProperty("energy").toString() << " energy!";
        return true;
        }
    else
        {
        qInfo() << "MUST DIE" << this->getAgent()->getProperty("@id").toString();
        return false;
        }

}

bool CheckAliveBehaviour::behave(){
    qInfo() << "IS DYING" << this->getAgent()->getProperty("@id").toString();
    this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
    GWSExecutionEnvironment::globalInstance()->unregisterAgent( this->getAgent() );
    return true;
}
