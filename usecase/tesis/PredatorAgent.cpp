#include "PredatorAgent.h"

#include "../../app/App.h"
#include "../../skill/view/ViewSkill.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../object/ObjectFactory.h"


PredatorAgent::PredatorAgent(QObject *parent) : GWSAgent( parent)
{

}


PredatorAgent::~PredatorAgent(){

}


void PredatorAgent::behaveOld()
{

    // Check your energy:
    if (this->getProperty("energy").toDouble() < 1.)
        {
        qInfo() << "RIP" << this->getProperty("@id").toString();
        this->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
        QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
        return;
        }

    else {
        qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
        qInfo() << "                 Wolf                           ";
        qInfo() << "                 ----                           ";

        GWSTimeEnvironment::globalInstance()->incrementAgentInternalTime( this , qrand() % 3 );

        /* Send information to website */
        emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


        /* Number of agents in the simulation (all types) */
        qInfo() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";


        qInfo() << "Initial energy = " << this->getProperty("energy");

        /* Move randomly through random index generator */
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
        int RandIndexY = qrand() % 3; //generates a random number between 0 and 2


        /* Move coordinates according to random index */
        int TargetX = direction[RandIndexX];
        int TargetY = direction[RandIndexY];

        // We assume a wolf will only nourish/breed when it moves to a different position.
        qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";

        if ((TargetX == 0) && (TargetY == 0))  /* Sometimes the wolf will "choose" to stay on same position */
           {
            qInfo() << "You choose to stay at the same position. You will eventually die of starvation.";
            this-> setProperty("energy", this->getProperty("energy").toDouble() * 0.6);
           }
        else  /* And sometimes it will "choose" to move */
           {
            // Move
            GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX, TargetY ) );

            //Moving consumes energy
            double initialEnergy = this->getProperty("energy").toDouble();
            double moveLosses = initialEnergy * 0.25;

            //Final energy of SheepAgent*/
            double finalEnergy = initialEnergy - moveLosses;
            this-> setProperty("energy", finalEnergy);

            // And see what is around you:
            QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
            qInfo() << "Cell Occupation = " << CellOccupation;

           int PredatorOccupation = 0;

           for (int i = 0; i < CellOccupation.size(); ++i)
                {
               if (CellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
                   {
                   // Eat!
                   qInfo() << "You get to eat!     ";
                   /* Feeding supplies energy */
                   double foodGains = 50.;
                   finalEnergy = finalEnergy + foodGains;
                   this-> setProperty("energy", finalEnergy);
                   qInfo() << "RIP" << CellOccupation.at(i)->getProperty("@id").toString();
                   CellOccupation.at(i)->setProperty("energy" , 0);
                   CellOccupation.at(i)->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
                   //QTimer::singleShot( 0 , CellOccupation.at(i) , &GWSAgent::deleteLater );
                   return;
                   }
               if (CellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
                  {
                   PredatorOccupation +=1;
                  }
                }


           qInfo() << "Wolves in your cell = " << PredatorOccupation;

                   if (PredatorOccupation == 1)
                       {
                       //Breed
                       qInfo() << "You get to breed! Another wolf in the GWSWorld!     ";

                       /* Breeding consumes energy */
                       this->setProperty("energy" , this->getProperty("energy").toDouble() * 0.5);

                       /* Add a cub to the World */
                       QJsonObject this_json = this->serialize();
                       this_json.insert( GWS_ID_PROP , QJsonValue::Undefined );
                       PredatorAgent* cubAgent = dynamic_cast<PredatorAgent*>( GWSObjectFactory::globalInstance()->fromJSON( this_json ) );
                       GWSExecutionEnvironment::globalInstance()->registerAgent( cubAgent );
                       qInfo() << "Cub's initial position = (" << GWSPhysicalEnvironment::globalInstance()->getGeometry( cubAgent )->getCentroid().getX() << "," << GWSPhysicalEnvironment::globalInstance()->getGeometry( cubAgent )->getCentroid().getY() << ")";

                       }

          }
          /*     // Wolves die when
               if (this->getProperty("energy") < 1.)
                  {
                    qInfo() << "RIP" << this->getProperty("@id").toString();
                    this->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
                    this->setProperty("energy" , 0);
                    //QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
                    return;
                  }*/
    }


}












