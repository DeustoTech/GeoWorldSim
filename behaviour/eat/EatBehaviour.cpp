#include "EatBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"



EatBehaviour::EatBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool EatBehaviour::finished(){
    return false;
}

bool EatBehaviour::behave(){

     qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid().toString();

     // Look what is around you:
     QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getAgent()->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
     qInfo() << "Cell Occupation = " << CellOccupation;
     qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid().toString();


     // We need to differentiate between preys: wolf kills sheep but sheep just reduces grass' energy:
     if (CellOccupation.size() == 0){
            qDebug() << "Nothing on this cell!";
            return true;
     } else {
        for (int i = 0; i < CellOccupation.size(); i++)
            {
            if (CellOccupation.at(i)->getProperty("@type").toString() == this->getProperty("prey").toString() )
               {

                // Eat
                qDebug() << "Found food!";
                double foodGains = CellOccupation.at(i)->getProperty("energy").toDouble();
                double finalEnergy = this->getAgent()->getProperty("energy").toDouble() + foodGains;
                this->getAgent()-> setProperty("energy", finalEnergy);
                emit GWSApp::globalInstance()->pushAgentSignal( this->getAgent()->serialize() );

                // Less grass = less energy of PastureAgent
                CellOccupation.at(i)->setProperty( "energy" , 0.1 );
                emit GWSApp::globalInstance()->pushAgentSignal( CellOccupation.at(i)->serialize() );

               /*if (this->getProperty("prey").toString() == "PastureAgent")
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
                  //CellOccupation.at(i)->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
                  }
               }*/
                }

            }


        }
        return true;

}
