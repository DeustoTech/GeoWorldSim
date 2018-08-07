#include "SheepAgent.h"


#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../skill/view/ViewSkill.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "TerrainAgent.h"

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {


}

SheepAgent::~SheepAgent(){

}

void SheepAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Sheep                           ";
    qInfo() << "                 -----                           ";

    GWSTimeEnvironment::globalInstance()->incrementAgentInternalTime( this , qrand() % 3 );

    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Number of agents in the simulation (all types) */
    qInfo() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";
    qInfo() << "I am" << this->getProperty("@id").toString();



    /* Move randomly through random index generator */
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
    int RandIndexY = qrand() % 3; //generates a random number between 0 and 2


    /* Move coordinates according to random index */
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];


    /* We assume a sheep will only nourish/breed when it moves to a different position. */
    qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";


    if ((TargetX == 0) && (TargetY == 0)) /* Sometimes the sheep will choose to stay on same position */
       {
       qInfo() << "You choose to stay at the same position. You will eventually die of starvation.";
       this-> setProperty("energy", this->getProperty("energy").toDouble() * 0.6);
       }
    else /* And sometimes they will choose to move */
       {
        // Move
        GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX, TargetY ) );
        //qInfo() << "Final position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid();

        // And see what is around you:
        QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
        qInfo() << "Cell Occupation = " << CellOccupation;

        int SheepOccupation = 0;

        for (int i = 0; i < CellOccupation.size(); i++)
            {
            if (CellOccupation.at(i)->getProperty("@type").toString() == "PastureAgent")
                {
                qInfo() << "Sheep's energy before eating = " << this->getProperty("energy");
                // Eat
                double foodGains = 1.;

                // Final energy of SheepAgent
                double finalEnergy = this->getProperty("energy").toDouble() + foodGains;
                this-> setProperty("energy", finalEnergy);

                /* Less grass = less energy of PastureAgent */
                CellOccupation.at(i)->setProperty( "energy", CellOccupation.at(i)->getProperty("energy").toDouble() - 1.0 );

                qInfo() << "Final sheep energy after eating = " << this->getProperty("energy");
                qInfo() << "Energy left in pasture = " << CellOccupation.at(i)->getProperty("energy");
                }

            if (CellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
               {
               SheepOccupation +=1;
               }

            }


            if (SheepOccupation == 1)
            {
            //Breed
            qInfo() << "You get to breed! Another sheep in the GWSWorld!     ";

            /* Breeding consumes energy */
            this->setProperty("energy" , this->getProperty("energy").toDouble() * 0.5);

            /* Add a lamb to the World */
            SheepAgent* lambAgent = new SheepAgent();
            GWSExecutionEnvironment::globalInstance()->registerAgent( lambAgent );

            /* Set lamb's properties */
            lambAgent->setProperty("energy", 20.);
            lambAgent->setProperty("@type", "SheepAgent");
            GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid() );
            lambAgent->icon_url = this->icon_url;
            }


    /* Sheep die when */
        if (this->getProperty("energy") < 1.)
           {
             qInfo() << "RIP" << this->getProperty("@id").toString();
             //terrain_agent->exit( this );
             QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
             return;
           }

    }
}



       /* Get target cell occupation through AgentGrid methods */
       //GWSCoordinate centroid = GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid();
       //QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue( centroid.getX() + TargetX, centroid.getY() + TargetY );


       /* Get what's around you through ViewSkill */


  /*     QList<GWSAgent*> agents = dynamic_cast<ViewSkill*>( this->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
       qInfo() << "Agents = " << agents;

       for (int i = 0; i < agents.size(); i++)
            {

            if (agents.at(i)->getProperty("@type").toString() == "PastureAgent"){
                // You found grass so move!
                GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( GWSPhysicalEnvironment::globalInstance()->getGeometry( agents.at(i) )->getCentroid() ) );
                /* Moving consumes energy */
//                double initialEnergy = this->getProperty("energy").toDouble();
  //              double moveLosses = initialEnergy / 4.0;
                /* Eating supplies energy */
  //              double foodGains = 30;
                /* Final energy of SheepAgent*/
 //               double finalEnergy = initialEnergy - moveLosses + foodGains;
 //               this-> setProperty("energy", finalEnergy);
                /* Less grass = less energy of PastureAgent */
  //              agents.at(i)->setProperty( "energy", agents.at(i)->getProperty("energy").toDouble() * 0.9 );

 //           }

  //          }

  //     int sheepOccupation = 0;



       /* Number of sheep in target cell */
       /*for (int i = 0; i < targetCellOccupation.size(); ++i)
           {
           if (!targetCellOccupation.at(i)->deleted && targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
              {
              sheepOccupation += 1;
              }
           if (!targetCellOccupation.at(i)->deleted && targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
              {
              /* You move to die*/
              //GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX , TargetY ) );

              //qInfo() << "Oh no! You become food...";

              /* Eating supplies energy to the PredatorAgent in question */
              //double foodGains = targetCellOccupation.at(i)->getProperty("energy").toDouble() / 2.0;

              /* Predator's final energy after eating */
              //double finalEnergy = targetCellOccupation.at(i)->getProperty("energy").toDouble() + foodGains;
              //targetCellOccupation.at(i)-> setProperty("energy", finalEnergy);

              /* Unregister the prey */
              //qInfo() << "RIP" << this->getProperty("@id").toString();

              //terrain_agent->exit( this );
              //QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
              //return;
              //}
           //}

       //qInfo()  << "Target cell SheepAgent occupation = " << sheepOccupation;


      /*if (sheepOccupation >= 2)
          {
          qInfo() << "You choose to move but target cell is too crowded with sheep! Try another direction or you will eventually die of starvation."  ;
          qInfo() << "";
          this-> setProperty("energy", this->getProperty("energy").toDouble()  - this->getProperty("energy").toDouble() /6.);
          qInfo() << "Energy = " << this->getProperty("energy").toDouble();
          }
       else
          {
          qInfo() << "Target cell not overbooked yet, you can move there!"  ;

          /* Move */
          /*GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX , TargetY ) );

          /* Moving consumes energy */
          /*double initialEnergy = this->getProperty("energy").toDouble();
          double moveLosses = this->getProperty("energy").toDouble() / 2.0;
          double finalEnergy = initialEnergy - moveLosses;
          qInfo() << "Energy after moving = " << finalEnergy;

          /* Eating supplies energy */
          /*double foodGains = 30;
          finalEnergy = finalEnergy + foodGains;
          qInfo() << "Energy after eating = " << finalEnergy;
          this-> setProperty("energy", finalEnergy);

          /* Moreover, if there is just another sheep at target -> Breed!*/
          /*if ((sheepOccupation == 1) && (this-> getProperty("energy").toDouble() >= 50.))
             {
             qInfo() << this->getProperty("@id").toString()<<", there is a living mate in your position! Move!";
             qInfo() << "You get to breed! Another sheep in the GWSWorld!     ";

             /* Breeding consumes energy */
            // this->setProperty("energy" , this->getProperty("energy").toDouble() / 2.0);

             /* Add a lamb to the World */
             /*SheepAgent* lambAgent = new SheepAgent();
             GWSEnvironment::globalInstance()->registerAgent( lambAgent );
             GWSExecutionEnvironment::globalInstance()->registerAgent( lambAgent );*/

             /* Set lamb's properties */
             /*lambAgent->setProperty("energy", 200.);
             lambAgent->setProperty("@type", "SheepAgent");
             GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid() );
             lambAgent->icon_url = this->icon_url;
             }*/


          //}


      //}


//}



/* NOTE: The following can be useful for counting purposes:
 *        GWSExecutionEnvironment::globalInstance()->getRunningAgents();
 *        GWSExecutionEnvironment::globalInstance()->getRunningAgentsByClass<SheepAgent>(SheepAgent::staticMetaObject.className());*/



