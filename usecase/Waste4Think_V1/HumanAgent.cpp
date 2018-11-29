#include "HumanAgent.h"

#include "../../environment/time_environment/TimeEnvironment.h"

HumanAgent::HumanAgent() : GWSAgent(){

    QTimer::singleShot( 1000 , [this](){
        GWSTimeEnvironment::globalInstance()->setAgentInternalTime( this->getSharedPointer() , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() + (qrand() % 600 * 1000) );
    } );

}



HumanAgent::~HumanAgent(){
}
