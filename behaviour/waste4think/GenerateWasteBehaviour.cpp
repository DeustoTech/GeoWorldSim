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

/* Formula for Zamudio waste generation based on Iraia's thesis:
   Data from UdalMap:
            |  2016 |  2015 |
      rest  |  9.56 |  9.52 |
      uni   | 20.16 | 20.13 |
    tasParo |  7.65 |  8.35 |
      urb   | 12.48 | 12.44 |*/


double GenerateWasteBehaviour::partialModel ( double rest, double uni, double tasParo , double urb){

    // Fracción resto kg / habitante / año:

    double model = 344.57 + 2.56 * rest  + 1.44 * uni - 1.42 * tasParo - 1.01 * urb;
    return model;
}




QStringList GenerateWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double daily_waste_amount = agent->getProperty( "kg_viviend").toDouble();
    //agent->setProperty( this->getProperty( STORE_GENERATED_WASTE_AS ).toString() , daily_waste_amount  );

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

    // Set waste generation by means of partial model:
    // 2016 data:
    double rest = 9.56 ;
    double uni = 20.16 ;
    double paro =  7.65;
    double urb = 12.48 ;
    double waste = GenerateWasteBehaviour::partialModel( rest , uni , paro , urb ) ;
    agent->setProperty( this->getProperty( STORE_GENERATED_WASTE_AS ).toString() , waste );

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();
    return nexts;

}


