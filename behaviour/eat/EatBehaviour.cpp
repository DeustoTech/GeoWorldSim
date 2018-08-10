#include "EatBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"


EatBehaviour::EatBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool EatBehaviour::finished(){
    return false;
}

bool EatBehaviour::behave(){

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
            if (CellOccupation.at(i)->getProperty("@type").toString() == this->getProperty("prey").toString() )
               {
               if (this->getProperty("prey").toString() == "PastureAgent")
                  {
                  // Eat
                  qDebug() << "Found pasture!";
                  double foodGains = CellOccupation.at(i)->getProperty("energy").toDouble();
                  double finalEnergy = this->getAgent()->getProperty("energy").toDouble() + foodGains;
                  this->getAgent()-> setProperty("energy", finalEnergy);

                  // Less grass = less energy of PastureAgent
                  CellOccupation.at(i)->setProperty( "energy" , 0.1 );
                  //return true;
                  }
               if (this->getProperty("prey").toString() == "SheepAgent")
                  {
                  // Eat
                  qDebug() << "Found sheep!";
                  double foodGains = CellOccupation.at(i)->getProperty("energy").toDouble() * 0.5;
                  double finalEnergy = this->getAgent()->getProperty("energy").toDouble() + foodGains;
                  this->getAgent()->setProperty("energy", finalEnergy);

                  qInfo() << "RIP" << CellOccupation.at(i)->getProperty("@id").toString();
                  CellOccupation.at(i)->setProperty("energy" , 0.);
                  CellOccupation.at(i)->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
                  }
               }
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
