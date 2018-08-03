#include "SheepAgent.h"

#include <QTimer>

#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "TerrainAgent.h"
#include <ctime>

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qInfo() << "SHEEP";
}

SheepAgent::~SheepAgent(){
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );
    terrain_agent->removeGridCellValue( this->getCentroid().getX() , this->getCentroid().getY() , this );
}

void SheepAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Sheep                           ";
    qInfo() << "                 -----                           ";


    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Number of agents in the simulation (all types) */
    qInfo() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";


    /* Register Terrain Agent so that we can add our sheep to a particular cell of the grid */
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );


    /* Get Sheep's cell_X and cell_y and original cell occupation */
    qInfo() << "I am" << this->property("@id").toString();
    qInfo() << "Original cell = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
    //qInfo() << "Occupation of original cell = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());


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
       qInfo() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
       qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
       qInfo() << "";
       }
    else /* And sometimes they will choose to move */
       {
       /* Get target cell occupation through AgentGrid methods */
       QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue( this->getCentroid().getX() + TargetX, this->getCentroid().getY() + TargetY );
       //qInfo()  << "Target cell occupation = " << targetCellOccupation;

       int sheepOccupation = 0;

       /* Number of sheep in target cell */
       for (int i = 0; i < targetCellOccupation.size(); ++i)
           {
           if (!targetCellOccupation.at(i)->deleted && targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
              {
              sheepOccupation += 1;
              }
           if (!targetCellOccupation.at(i)->deleted && targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
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

              qInfo() << "Oh no! You become food...";

              /* Eating supplies energy to the PredatorAgent in question */
              double foodGains = targetCellOccupation.at(i)->getProperty("energy").toDouble() / 2.0;

              /* Predator's final energy after eating */
              double finalEnergy = targetCellOccupation.at(i)->getProperty("energy").toDouble() + foodGains;
              targetCellOccupation.at(i)-> setProperty("energy", finalEnergy);

              /* Unregister the prey */
              qInfo() << "RIP" << this->property("@id").toString();
              terrain_agent->removeGridCellValue( this->getCentroid().getX(), this->getCentroid().getY(), this);
              QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
              return;
              }
           }

       qInfo()  << "Target cell SheepAgent occupation = " << sheepOccupation;


       if (sheepOccupation >= 2)
          {
          qInfo() << "You choose to move but target cell is too crowded with sheep! Try another direction!"  ;
          qInfo() << "";
          //return;
          }
       else
          {
          qInfo() << "Target cell not overbooked yet, you can move there!"  ;

          /* Notify the grid that the sheep is leaving */
          terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

          /* Move */
          this->transformMove( GWSCoordinate( TargetX , TargetY ) );

          /* Final position of the agent*/
          qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

          /* Notify the grid of the sheep's new position */
          terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
          //qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

          /* Moving consumes energy */
          double initialEnergy = this->getProperty("energy").toDouble();
          double moveLosses = this->getProperty("energy").toDouble() / 8.0;
          double finalEnergy = initialEnergy - moveLosses;
          qInfo() << "Energy after moving = " << finalEnergy;

          /* Eating supplies energy */
          double foodGains = finalEnergy / 12.0;
          finalEnergy = finalEnergy + foodGains;
          qInfo() << "Energy after eating = " << finalEnergy;
          this-> setProperty("energy", finalEnergy);

          /* Moreover, if there is just another sheep at target -> Breed!*/
          if (sheepOccupation == 1)
             {
             qInfo() << this->property("@id").toString()<<", there is a living mate in your position! Move!";
             qInfo() << "   You get to breed! Another sheep in the GWSWorld!     ";

             /* Breeding consumes energy */
             this->setProperty("energy" , this->getProperty("energy").toDouble() / 2.0);

             /* Add a lamb to the World */
             SheepAgent* lambAgent = new SheepAgent();
             GWSExecutionEnvironment::globalInstance()->registerAgent(lambAgent);
             GWSAgentEnvironment::globalInstance()->registerAgent( lambAgent );

             /* Set lamb's properties */
             lambAgent->setProperty("energy", 10);
             lambAgent->setProperty("@type", "SheepAgent");
             lambAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );

             /* Notify the grid of new lamb's position */
             terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), lambAgent);
             }



           /* Sheep die when */
               if (this->property("energy") < 1.)
                  {
                    qInfo() << "RIP" << this->property("@id").toString();
                    terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
                    QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
                    return;
                  }
          }


      }


}



/* NOTE: The following can be useful for counting purposes:
 *        GWSExecutionEnvironment::globalInstance()->getRunningAgents();
 *        GWSExecutionEnvironment::globalInstance()->getRunningAgentsByClass<SheepAgent>(SheepAgent::staticMetaObject.className());*/



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
