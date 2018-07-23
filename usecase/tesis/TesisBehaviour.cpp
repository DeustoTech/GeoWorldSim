#include "TesisBehaviour.h"

QString TesisBehaviour::COUTDOWN_PROP = "ticks_left";

TesisBehaviour::TesisBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
    this->setProperty( TesisBehaviour::COUTDOWN_PROP , 3 );
}

bool TesisBehaviour::finished(){
    return (this->getProperty( COUTDOWN_PROP ).toInt() <= 0);
}

bool TesisBehaviour::behave(){
    this->getAgent()->incrementBusy();

    qDebug() << 1;
    this->setProperty( COUTDOWN_PROP , this->getProperty( COUTDOWN_PROP ).toInt() - 1 );
    this->getAgent()->incrementInternalTime( 1 );

    this->getAgent()->decrementBusy();
    return true;
}
