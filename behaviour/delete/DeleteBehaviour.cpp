#include "DeleteBehaviour.h"

#include "../../app/App.h"
#include "../../environment/Environment.h"

DeleteBehaviour::DeleteBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){

}

bool DeleteBehaviour::finished(){
    return deleted;
}

bool DeleteBehaviour::behave(){

    this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/250/250162.svg";

    this->getAgent()->incrementInternalTime( 1 );

    QTimer::singleShot( 10*1000 , this->getAgent() , &GWSAgent::deleteLater ); // Die in 10 seconds
    return this->deleted = true;
}
