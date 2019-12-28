#include "BreedBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"


BreedBehaviour::BreedBehaviour() : GWSBehaviour(){
}

bool BreedBehaviour::finished(){
    return false;
}

bool BreedBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    // Look what is around you:

     QList< QSharedPointer<GWSAgent> > CellOccupation = agent->getSkill( ViewSkill::staticMetaObject.className() ).dynamicCast<ViewSkill>()->getViewingAgents();
     qInfo() << "Cell Occupation = " << CellOccupation.size();

     int Congeners = 0;

    for (int i = 0; i < CellOccupation.size(); i++)
        {
        if (CellOccupation.at(i)->getProperty("@type") == agent->getProperty("@type"))
           {
           Congeners +=1;
           }
        }

     qInfo() << "Congeners in your cell = " << Congeners;

     if (Congeners == 2)
         {
         //Breed
         qInfo() << "Found mate! ";

         // Breeding consumes energy
         agent->setProperty("energy" , agent->getProperty("energy").toDouble() * 0.5);

         //Add offspring to the World
         QJsonObject this_json = agent->serialize();
         this_json.insert( GWS_ID_PROP , QJsonValue::Undefined );
         this_json.insert( "@behaviours" , QJsonValue::Undefined );
         this_json.insert( "@skills" , QJsonValue::Undefined );
         this_json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , this_json.value( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble() + 10000 );

         // Add skills
         QList< QSharedPointer<GWSSkill> > skills = agent->getSkills( GWSSkill::staticMetaObject.className() );
         if ( !skills.isEmpty() ) {
            QJsonArray arr;
            foreach ( QSharedPointer<GWSSkill> o , skills ){
                    arr.append( o->serialize() );
                    }
            this_json.insert( "@skills" , arr );
            }

         // Add behaviours
         QList< QSharedPointer<GWSBehaviour> > behaviours = agent->getBehaviours( GWSBehaviour::staticMetaObject.className() );
         if ( !behaviours.isEmpty() ) {
            QJsonArray arr;
            foreach ( QSharedPointer<GWSBehaviour> o , behaviours ){
                    arr.append( o->serialize() );
                    }
            this_json.insert( "@behaviours" , arr );
            }

         // Create agent
         QSharedPointer<GWSAgent> OffspringAgent = GWSObjectFactory::globalInstance()->fromJSON( this_json ).dynamicCast<GWSAgent>();

         // Change icons to see better
         if (OffspringAgent->getProperty("@type") == "SheepAgent")
            {
            OffspringAgent->icon_url = "https://image.flaticon.com/icons/svg/375/375057.svg";
            }

         if (OffspringAgent->getProperty("@type") == "PredatorAgent")
            {
             OffspringAgent->icon_url = "https://image.flaticon.com/icons/svg/235/235018.svg";
            }

         return true;
         }
     else
        {
         qInfo() << "No mating";
         return true;
        }

}
