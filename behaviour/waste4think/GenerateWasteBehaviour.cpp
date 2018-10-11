#include "GenerateWasteBehaviour.h"

#include "../../app/App.h"

QString GenerateWasteBehaviour::WASTE_TYPE = "waste_type";
QString GenerateWasteBehaviour::FAMILY_MEMBERS = "family_members";
QString GenerateWasteBehaviour::MAX_VALUE = "max_waste";
QString GenerateWasteBehaviour::NEXTS_IF_FULL = "nexts_if_full";
QString GenerateWasteBehaviour::NEXTS_IF_NOT_FULL = "nexts_if_not_full";

GenerateWasteBehaviour::GenerateWasteBehaviour() : GWSBehaviour(){
}

/************************************************************
 * METHODS
 ***********************************************************/

QStringList GenerateWasteBehaviour::behave(){

    /*QSharedPointer<GWSAgent> agent = this->getAgent();
    QVariant value = agent->getProperty( this->getProperty( WASTE_TYPE ).toString() );
    QVariant max_value = this->getProperty( MAX_VALUE);

    // Keep incrementing
    if ( value < max_value ){

        QVariant incremented = value.toDouble() + this->getProperty( FAMILY_MEMBERS ).toDouble();
        if( max_value.isValid() ){ incremented = qMin( max_value , incremented ); }
        agent->setProperty( this->getProperty( WASTE_TYPE ) , incremented );
        QStringList next = this->getProperty( NEXTS_IF_NOT_FULL);
        emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

        return next;
    }
    // Your are full, go to next behaviour
    if ( value >= max_value  ){
        QStringList next = this->getProperty( NEXTS_IF_FULL);
        return next;
    }*/

}
