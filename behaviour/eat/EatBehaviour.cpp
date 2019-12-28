#include "EatBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"

#include <QJsonDocument>

#include "../../skill/view/ViewSkill.h"

EatBehaviour::EatBehaviour() : GWSBehaviour(){
}


bool EatBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

     // Look what is around you:
     QList< QSharedPointer<GWSAgent> > CellOccupation = agent->getSkill( ViewSkill::staticMetaObject.className() ).dynamicCast<ViewSkill>()->getViewingAgents();
     qDebug() << CellOccupation;
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
                qDebug() << "Initial energy = " << agent->getProperty("energy");
                double foodGains = CellOccupation.at(i)->getProperty("energy").toDouble();
                double finalEnergy = agent->getProperty("energy").toDouble() + foodGains;
                agent->setProperty("energy", finalEnergy);
                qDebug() << "Final energy = " << agent->getProperty("energy");
                // Less grass = less energy of PastureAgent
                CellOccupation.at(i)->setProperty( "energy" , 0.1 );

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
