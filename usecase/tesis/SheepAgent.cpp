#include "SheepAgent.h"
#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "TerrainAgent.h"
#include <ctime>

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "SHEEP";
}


void SheepAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Sheep                           ";
    qInfo() << "                 -----                           ";


    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Number of agents in the simulation (all types).
    /* NOTE: The following is equivalent to
     * GWSExecutionEnvironment::globalInstance()->getRunningAgentsAmount() */
    qDebug() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";


    /* NOTE: The following can be useful for counting purposes:
     *        GWSExecutionEnvironment::globalInstance()->getRunningAgents();
     *        GWSExecutionEnvironment::globalInstance()->getRunningAgentsByClass<SheepAgent>(SheepAgent::staticMetaObject.className());*/


    /* Register Terrain Agent so that we can add our sheep to a particular cell of the grid */
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );


    /* Get Sheep's cell_X and cell_y and original cell occupation */
    qInfo() << "I am" << this->property("@id").toString();
    qInfo() << "Original cell = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
    qInfo() << "Occupation of original cell = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());


    /* Move randomly through random index generator */
    srand ( time(NULL) );
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = rand() % 3; //generates a random number between 0 and 2
    int RandIndexY = rand() % 3; //generates a random number between 0 and 2

    qDebug() << "RandIndexX = " << RandIndexX;
    qDebug() << "RandIndexY = " << RandIndexY;

    /* Move coordinates according to random index */
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];


    /* We assume a sheep will only nourish/breed when it moves to a different position. */
    qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";


    if ((TargetX == 0) && (TargetY == 0)) /* Sometimes the sheep will choose to stay on same position */
       {
       qInfo() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
       qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
       qInfo() << "";
       }
    else /* And sometimes they will choose to move */
       {
       /* Get target cell occupation through AgentGrid methods */
       QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue(this->getCentroid().getX() + TargetX, this->getCentroid().getY() + TargetY  );
       qInfo()  << "Target cell occupation = " << targetCellOccupation;

       int sheepOccupation = 0;

       /* Number of sheep in target cell */
       for (int i = 0; i < targetCellOccupation.size(); ++i)
           {
           if (targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
              {
              sheepOccupation += 1;
              }
           if (targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
              {
              /* You move to die*/

              /* Notify the grid that the sheep is leaving */
              terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

              /* Move */
              this->transformMove( GWSCoordinate( TargetX , TargetY ) );

              /* Final position of the sheep */
              qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

              /* Notify the grid of the sheep's new position */
              terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
              qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

              qDebug() << "****************************";
              qDebug() << "   Oh no! You become food...";
              qDebug() << "****************************";

              /* Eating supplies energy to the PredatorAgent in question */
              float foodGains = targetCellOccupation.at(i)->getProperty("energy").toFloat() / 2.0;

              /* Predator's final energy after eating */
              float finalEnergy = targetCellOccupation.at(i)->getProperty("energy").toFloat() + foodGains;
              targetCellOccupation.at(i)-> setProperty("energy", finalEnergy);
              qInfo() << "Wolf's energy after eating = " << finalEnergy;

              /* Unregister the prey */
              qInfo() << "RIP" << this->property("@id").toString();
              terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
              GWSExecutionEnvironment::globalInstance()->unregisterAgent( this );
              qDebug() << "Target cell occupation after eating = " << terrain_agent->getGridCellValue(targetCellOccupation.at(i)->getCentroid().getX(), targetCellOccupation.at(i)->getCentroid().getY());
              return;
              }
           }

       qInfo()  << "Target cell SheepAgent occupation = " << sheepOccupation;

       /* Modify behaviour based on target cell occupation
        * If there are 2 or more (max of 3 considering that
        * offspring is generated in the same cell)
        * sheep in the target cell, do nothing. */

       if (sheepOccupation >= 2)
          {
          qInfo() << "You choose to move but target cell is too crowded with sheep! Try another direction!"  ;
          qInfo() << "";
          return;
          }
       else
          {
          qInfo() << "Target cell not overbooked yet, you can move there!"  ;

          // Notify the grid that the sheep is leaving:
          terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

          // Move
          this->transformMove( GWSCoordinate( TargetX , TargetY ) );

          // Final position of the agent:
          qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

          // Notify the grid of the sheep's new position:
          terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
          qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

          // Final energy after moving:
          float finalEnergy = this->getProperty("energy").toFloat() - this->getProperty("energy").toFloat() / 8.0;
          qInfo() << "Energy after moving = " << finalEnergy;

          // Eating supplies energy:
          float foodGains = finalEnergy / 12.0;

          // The final energy after eating:
          finalEnergy = finalEnergy + foodGains;
          qInfo() << "Energy after eating = " << finalEnergy;

          this-> setProperty("energy", finalEnergy);

          // Moreover, if there is just another sheep at target -> Breed!
          if (sheepOccupation == 1)
             {
             qInfo() << this->property("@id").toString()<<", there is a living mate in your position! ";

             /*
              *  Reproductive constraints:
              *   - Internal time threshold
              *   - Energy threshold
              */

             // TO BE CONSIDERED!

             /*
              *  Breed!
              */
             qInfo() << "************************************************************";
             qInfo() << "   YAS! You get to breed! Another sheep in the GWSWorld!     ";
             qInfo() << "************************************************************";

             this->setProperty("energy" , this->getProperty("energy").toFloat() / 2.0);

             // Welcome a lamb to the World:
             SheepAgent* lambAgent = new SheepAgent();
             GWSExecutionEnvironment::globalInstance()->registerAgent(lambAgent);
             GWSAgentEnvironment::globalInstance()->registerAgent( lambAgent );
             lambAgent->setProperty("energy", 10);
             lambAgent->setProperty("@type", "SheepAgent");
             lambAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );
             qDebug() << "Lamb position = (" << lambAgent->getCentroid().getX() << ", " << lambAgent->getCentroid().getY() << ")";

             // Notify the grid of new sheep's position:
             terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), lambAgent);
             qDebug() << "Cell occupation after breeding = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

             /*  Reproductive constraints:
              *  - Set internal time counter to 0 */
             }



           // Sheep die when:
               if (this->property("energy") < 1)
                  {
                  GWSExecutionEnvironment::globalInstance()->unregisterAgent( this );
                  terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
                  qInfo() << "****************************************";
                  qInfo() << "RIP" << this->property("@id").toString();
                  qInfo() << "****************************************";
                  }
            return;
          }


      }


}





/*
 *  Generate a list with all the sheeps in the GWS world.
 *  This list allows us to loop over all the existing sheeps
 *  and to check whether they share the same position!

// Loop to see how many sheep are at the target point:
for (int i = 0; i < sheeps.size(); ++i)
    {
    if (
       // There is a SheepAgent that shares your X coord
       (sheeps.at(i)->getCentroid().getX() == this->getCentroid().getX() + TargetX)
       && // ... and your Y coord
       (sheeps.at(i)->getCentroid().getY() == this->getCentroid().getY() + TargetY)
       && // ... and it is not yourself
       (sheeps.at(i)->property("@id") != this->property("@id"))
       && // ... and it is not dead
       (sheeps.at(i)->property("running") == "true"))
       {
       // Increase the occupation
       occupation += 1;
       }
    }

qDebug() << "Target cell SheepAgent occupation = " << occupation;
*/
