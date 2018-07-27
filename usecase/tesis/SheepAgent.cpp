#include "SheepAgent.h"
#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "PASO";
}


void SheepAgent::behave()
{
        qDebug() << "-------------------------------------------------";

        // Send information to website
        emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );

        /*
         *  Generate a list with all the sheeps in the GWS world.
         *  This list allows us to loop over all the existing sheeps
         *  and to check whether they share the same position!
         */
        QList<GWSAgent*> sheeps = GWSAgentEnvironment::globalInstance()->getByClass( SheepAgent::staticMetaObject.className() );
        qDebug() << "Number of running SheepAgents in the field = "<< sheeps.size() ;

        // Get cell_X and cell_y
        qDebug() << "I am" << this->property("@id").toString();
        qDebug() << "Initial position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")";

        // Move randomly through random index generator:

        srand ( time(NULL) ); //initialize the random seed
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = rand() % 3; //generates a random number between 0 and 2
        int RandIndexY = rand() % 3; //generates a random number between 0 and 2

        // Move coordinates either 0, 1 or -1 points
        // TODO: final position has to be within the grid!
        int TargetX = this->getProperty("cell_x").toInt() + direction[RandIndexX];
        int TargetY = this->getProperty("cell_y").toInt() + direction[RandIndexY];
        int occupation = 0;


        // The sheep will stay on same position:
        if ((direction[RandIndexX] == 0) && (direction[RandIndexY] == 0))
           {
            /*
             *  We assume a sheep will only nourish when it moves to a different position.
             *  Same applies to breeding.
             */
            qDebug() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
           }

        else
           {
           qDebug() << "Target cell = ("<< TargetX << "," << TargetY<< ")";
           // Loop to see how many sheep are at the target point:

           for (int i = 0; i < sheeps.size(); ++i)
                {
                if (
                     // There is a SheepAgent that shares your X coord
                     (sheeps.at(i)->property("cell_x") == TargetX)
                     && // ... and your Y coord
                     (sheeps.at(i)->property("cell_y") == TargetY)
                     && // ... and it is not yourself
                     (sheeps.at(i)->property("@id") != this->property("@id"))
                     && // ... and it is not dead
                     (sheeps.at(i)->property("running") == "true"))
                        {
                        // Increase the occupation
                        occupation += 1;
                        }
                }

            qDebug() << "Target cell occupation = " << occupation;

            // Modify behaviour based on target cell occupation

            // If there are 2 or more (max of 3 considering that offspring is generated in the same cell
            // sheep in the target cell, do nothing.

            if (occupation == 2)
               {
               qDebug() << "Target is too crowded, you would be a third wheel! Try another direction!"  ;
               }
            if (occupation == 3)
               {
               qDebug() << "There is a family at the target position! Try another direction!"  ;
               }

            else
               {
               //if
               qDebug() << "Target not overbooked yet, you can move there"  ;

               this->setProperty("cell_x", this->getProperty("cell_x").toInt() + direction[RandIndexX]);
               this->setProperty("cell_y", this->getProperty("cell_y").toInt() + direction[RandIndexY]);

               // Final position of the agent:
               qDebug() << "Final position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")";

               // Get initial energy
               float initialEnergyFloat = this->getProperty("energy").toFloat();

               // Moving consumes energy:
               float moveLosses = initialEnergyFloat / 8.0;

               // The final energy after moving:
               float finalEnergy = initialEnergyFloat - moveLosses;
               qDebug() << "Energy after moving = " << finalEnergy;

               // Eating supplies energy:
               float foodGains = finalEnergy / 12.0;

               // The final energy after eating:
               finalEnergy = finalEnergy + foodGains;
               qDebug() << "Energy after eating = " << finalEnergy;

               this-> setProperty("energy", finalEnergy);

               // Iterate over list to see where are the sheep:
               //for (int i = 0; i < sheeps.size(); ++i)

               //{

               // Position and suitability constraints
               if (occupation == 1)
                  {
                  qDebug() << this->property("@id").toString()<<", there is a living mate in your position!" ;


                  /*
                   *  Reproductive constraints:
                   *   - Internal time threshold
                   *   - Energy threshold
                   */

                  // TO BE CONSIDERED!

                  /*
                   *  Breed!
                   */
                  qDebug() << "**********************";
                  qDebug() << "YAS! You get to breed!";
                  qDebug() << "**********************";
                  this->setProperty("energy" , this->getProperty("energy").toFloat() / 2.0);
                  //sheeps.at(i)->setProperty("energy" , sheeps.at(i)->getProperty("energy").toFloat() / 2.0);

                  // Welcome a lamb to the World:

                  SheepAgent* lambAgent = new SheepAgent();
                  GWSExecutionEnvironment::globalInstance()->registerAgent(lambAgent);
                  GWSAgentEnvironment::globalInstance()->registerAgent( lambAgent );
                  lambAgent->setProperty("energy", 10);
                  lambAgent->setProperty("cell_x", this->property("cell_x"));
                  lambAgent->setProperty("cell_y", this->property("cell_y"));


                  //  Reproductive constraints:
                  //  - Set internal time counter to 0

                  }
               }

           }


        // Sheep die when:
        if (this->property("energy") < 0.01)
            {
            this->setProperty(RUNNING_PROP, "FALSE");
            qDebug() << "RIP" << this->property("@id").toString();
            }

}



/* internal_clock = this->getInternalTime();
   if internal_clock >= reproduction_tick_threshold && another_sheep_in_cell;
        {
 *      SheepAgent::Reproduce(  );
 *      GWSBehaviour::getNext();
 *      }
   void SheepAgent::Reproduce(  )
        {
        energy = this->getProperty( SheepAgent::ENERGY_PROPERTY );
        this->setProperty( SheepAgent::ENERGY_PROPERTY , energy -= energy / 2.0);
        SheepAgent* lamb = new SheepAgent();
        lambEnergy = getProperty(lamb::ENERGY_PROPERTY)
        lamb->setProperty(SheepAgent::ENERGY_PROPERTY, lambEnergy == this->getProperty( SheepAgent::ENERGY_PROPERTY )/ 2.0)
        GWSBehaviour::getNext();
        }
*/



/*for (int i = 0; i < sheeps.size(); ++i)
    {
    if
      (
      // ... you share my X coordinate
      (sheeps.at(i)->property("cell_x") == this->property("cell_x"))
      && // ... and my Y coordinate
      (sheeps.at(i)->property("cell_y") == this->property("cell_y"))
      && // ... and you are not myself
      (sheeps.at(i)->property("@id") != this->property("@id"))
      && // ... and you are not dead
      (sheeps.at(i)->property("running") == "true")
      )
      {*/


