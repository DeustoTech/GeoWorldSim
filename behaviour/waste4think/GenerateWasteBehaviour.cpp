#include "GenerateWasteBehaviour.h"

#include "../../app/App.h"

QString GenerateWasteBehaviour::WASTE_TYPE = "waste_type";
QString GenerateWasteBehaviour::FAMILY_MEMBERS = "family_members";
QString GenerateWasteBehaviour::MAX_VALUE = "max_waste";
QString GenerateWasteBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString GenerateWasteBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString GenerateWasteBehaviour::STORE_GENERATED_WASTE_AS = "store_generated_waste_as";

GenerateWasteBehaviour::GenerateWasteBehaviour() : GWSBehaviour(){
}

/************************************************************
 * METHODS
 ***********************************************************/

QStringList GenerateWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    //double initial_waste = 0;
    double daily_waste_amount = agent->getProperty( "kg_viviend").toDouble();
    //QVariant waste_type = this->getProperty( WASTE_TYPE );
    agent->setProperty( this->getProperty( STORE_GENERATED_WASTE_AS ).toString() , daily_waste_amount  );


    /*QVariant max_value = this->getProperty( MAX_VALUE);

    // Keep incrementing
    if ( agent_waste_amount < max_value ){

        QVariant waste_incremented = agent_waste_amount.toDouble() + this->getProperty( FAMILY_MEMBERS ).toDouble();
        if( max_value.isValid() ){ waste_incremented = qMin( max_value , waste_incremented ); }

        // agent->setProperty( waste_type , waste_incremented );
        agent->setProperty( STORE_GENERATED_WASTE_AS , waste_incremented );
        QStringList next = this->getProperty( NEXTS_IF_FALSE ).toStringList();

        emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
        return next;
    }
    // Your are full, go to next behaviour
    if ( agent_waste_amount >= max_value  ){
        QStringList next = this->getProperty( NEXTS_IF_TRUE ).toStringList();
        return next;
    }*/

    QStringList nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();
    return nexts;

}
