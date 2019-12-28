#include "DetermineAgentTypeBehaviour.h"
#include "../../object/ObjectFactory.h"
#include "../../behaviour/wood/CompostingFacilityBehaviour.h"
#include "../../behaviour/wood/TimberFactoryBehaviour.h"
#include "../../behaviour/wood/TreeFarmBehaviour.h"
#include "../../app/App.h"


DetermineAgentTypeBehaviour::DetermineAgentTypeBehaviour() : GWSBehaviour()
{

}



bool DetermineAgentTypeBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    this->input_resource = GWSResourceUnit( agent->getProperty("input_resource").toString() );

    QString behaviour;


    if ( this->input_resource.type == "Wood" ){
        agent->setProperty( "output_resource", "Sawdust" );
        agent->setProperty( "@type", "TimberFactoryAgent" );

        GWSObjectFactory::globalInstance()->registerType( TimberFactoryBehaviour::staticMetaObject );
        behaviour = "TimberFactoryBehaviour";
        agent->icon_url = "https://image.flaticon.com/icons/svg/972/972755.svg";


    if ( this->input_resource.type == "Sawdust" ){
        agent->setProperty( "output_resource", "Compost" );
        agent->setProperty( "@type", "CompostingFacilityAgent");

        GWSObjectFactory::globalInstance()->registerType( CompostingFacilityBehaviour::staticMetaObject );
        behaviour = "CompostingFacilityBehaviour";
        agent->icon_url = "https://image.flaticon.com/icons/svg/306/306655.svg";
    }

    if ( this->input_resource.type == "Compost"){
        agent->setProperty( "output_resource", "Wood" );
        agent->setProperty( "@type", "TreeFarmAgent");

        GWSObjectFactory::globalInstance()->registerType( TreeFarmBehaviour::staticMetaObject );
        behaviour = "WoodFarmBehaviour";
        agent->icon_url = "https://image.flaticon.com/icons/svg/1132/1132266.svg";
    }

    QJsonObject this_json = agent->serialize();
    this_json.insert( "@behaviours" , behaviour );
    //this_json.insert( "@skills" , skill );

    emit GWSApp::globalInstance()->sendAgentSignal( agent ->serialize() );


}
}
