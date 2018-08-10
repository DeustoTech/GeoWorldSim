#include "BreedBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"


BreedBehaviour::BreedBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool BreedBehaviour::finished(){
    return false;
}

bool BreedBehaviour::behave(){

     qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry(this->getAgent() )->getCentroid().toString();


     // Look what is around you:
     QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getAgent()->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
     qInfo() << "Cell Occupation = " << CellOccupation;

     qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry(this->getAgent() )->getCentroid().toString();


     int Occupation = 0;

     // We need to differentiate between preys: wolf kills sheep but sheep just reduces grass' energy:
     if (CellOccupation.size() == 0)
        {
        qDebug() << "Nothing on this cell!";
        return true;
        }
     else
        {
        for (int i = 0; i < CellOccupation.size(); i++)
            {
            if (CellOccupation.at(i)->getProperty("@type") == this->getAgent()->getProperty("@type"))
               {
               Occupation +=1;
               }

            }

            qInfo() << "Congeners in your cell = " << Occupation;

            if (Occupation >= 1)
               {
               //Breed
               qInfo() << "Found mate! ";

               // Breeding consumes energy
               this->getAgent()->setProperty("energy" , this->getAgent()->getProperty("energy").toDouble() * 0.5);

               //Add offspring to the World
               /*QJsonObject this_json = this->getAgent()->serialize();
               this_json.insert( GWS_ID_PROP , QJsonValue::Undefined );
               this_json.insert( "@behaviours" , QJsonValue::Undefined );
               this_json.insert( "@skills" , QJsonValue::Undefined );

               // Create non skilled and non behavioured agent
               GWSAgent* OffspringAgent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( this_json ) );

               QList<GWSSkill*> skills = this->getAgent()->getSkills( GWSSkill::staticMetaObject.className() );
               if( !skills.isEmpty() ){
                   foreach( GWSSkill* o , skills ){
                       GWSSkill* copy = dynamic_cast<GWSSkill*>( GWSObjectFactory::globalInstance()->fromJSON( o->serialize() , OffspringAgent ) );
                       OffspringAgent->addSkill( copy );
                   }
               }
               QList<GWSBehaviour*> behaviours = this->getAgent()->getBehaviours( GWSBehaviour::staticMetaObject.className() );
               if( !behaviours.isEmpty() ){
                   QJsonArray arr;
                   foreach( GWSBehaviour* o , behaviours ){
                       qDebug() << this->thread() << o->thread();
                       arr.append( o->serialize() );
                   }
                   this_json.insert( "@behaviours" , arr );
               }*/

               //GWSExecutionEnvironment::globalInstance()->registerAgent( OffspringAgent );
               //qInfo() << "OffspringAgent's initial position = (" << GWSPhysicalEnvironment::globalInstance()->getGeometry( OffspringAgent )->getCentroid().getX() << "," << GWSPhysicalEnvironment::globalInstance()->getGeometry( OffspringAgent )->getCentroid().getY() << ")";

              }

            }

}
