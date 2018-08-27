#include "BreedBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"


BreedBehaviour::BreedBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool BreedBehaviour::finished(){
    return false;
}

bool BreedBehaviour::behave(){

     qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry(this->getAgent() )->getCentroid().toString();

     // Look what is around you:
     QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getAgent()->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
     qInfo() << "Cell Occupation = " << CellOccupation.size();

     int Congeners = 0;

    for (int i = 0; i < CellOccupation.size(); i++)
        {
        if (CellOccupation.at(i)->getProperty("@type") == this->getAgent()->getProperty("@type"))
           {
           Congeners +=1;
           }
        }

     qInfo() << "Congeners in your cell = " << Congeners;

     if (Congeners >= 2)
         {
         //Breed
         qInfo() << "Found mate! ";

         // Breeding consumes energy
         this->getAgent()->setProperty("energy" , this->getAgent()->getProperty("energy").toDouble() * 0.5);

         //Add offspring to the World
         QJsonObject this_json = this->getAgent()->serialize();
         this_json.insert( GWS_ID_PROP , QJsonValue::Undefined );
         this_json.insert( "@behaviours" , QJsonValue::Undefined );
         this_json.insert( "@skills" , QJsonValue::Undefined );
         this_json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , this_json.value( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble() + 10000 );

         // Add skills
         QList<GWSSkill*> skills = this->getAgent()->getSkills( GWSSkill::staticMetaObject.className() );
         if ( !skills.isEmpty() )
            {
            QJsonArray arr;
            foreach ( GWSSkill* o , skills ){
                    arr.append( o->serialize() );
                    }
            this_json.insert( "@skills" , arr );
            }

         // Add behaviours
         QList<GWSBehaviour*> behaviours = this->getAgent()->getBehaviours( GWSBehaviour::staticMetaObject.className() );
         if ( !behaviours.isEmpty() )
            {
            QJsonArray arr;
            foreach ( GWSBehaviour* o , behaviours ){
                    arr.append( o->serialize() );
                    }
            this_json.insert( "@behaviours" , arr );
            }

         // Create agent
         GWSAgent* OffspringAgent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( this_json ) );

         // Change icons to see better
         if (OffspringAgent->getProperty("@type") == "SheepAgent")
            {
            OffspringAgent->icon_url = "https://image.flaticon.com/icons/svg/375/375057.svg";
            }

         if (OffspringAgent->getProperty("@type") == "PredatorAgent")
            {
             OffspringAgent->icon_url = "https://image.flaticon.com/icons/svg/235/235018.svg";
            }

         GWSExecutionEnvironment::globalInstance()->registerAgent( OffspringAgent );

         qInfo() << "OffspringAgent's initial position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry(OffspringAgent )->getCentroid().toString();
         qInfo() << "Agents in cell = " << dynamic_cast<ViewSkill*>( OffspringAgent->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents().size();

         return true;
         }
     else
        {
         qInfo() << "No mate on your position";
         return true;
        }

}
