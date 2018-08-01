#include "SheepAgent.h"
#include "math.h"
#include <algorithm>    // std::count_if
#include "../../app/App.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "SHEEP";
}


void SheepAgent::behave()
{
        qDebug() << "--------------- Sheep ---------------------------";

        // Send information to website
        emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


        qDebug() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";

        /*
         *  Generate a list with all the sheeps in the GWS world.
         *  This list allows us to loop over all the existing sheeps
         *  and to check whether they share the same position!
         */
        QList<GWSAgent*> sheeps = GWSAgentEnvironment::globalInstance()->getByClass( SheepAgent::staticMetaObject.className() );
        qDebug() << "Number of running SheepAgents in the field = "<< sheeps.size() ;
        // Get cell_X and cell_y
        qDebug() << "I am" << this->property("@id").toString();
        qDebug() << "Initial position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

        // Move randomly through random index generator:
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = rand() % 3; //generates a random number between 0 and 2
        int RandIndexY = rand() % 3; //generates a random number between 0 and 2

        // Move coordinates either 0, 1 or -1 points
        // TODO: final position has to be within the grid!
        int TargetX = direction[RandIndexX];
        int TargetY = direction[RandIndexY];
        int occupation = 0;


        // The sheep will stay on same position:
        if ((TargetX == 0) && (TargetY == 0))
           {
            /*
             *  We assume a sheep will only nourish when it moves to a different position.
             *  Same applies to breeding.
             */
            qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";
            qInfo() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
            qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
           }

        else
           {
           qDebug() << "Target movement = ("<< TargetX << "," << TargetY<< ")";

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

            /*
             * Modify behaviour based on target cell occupation
             * If there are 2 or more (max of 3 considering that offspring is generated in the same cell)
             * sheep in the target cell, do nothing.
             */

            if (occupation == 2)
               {
               qDebug() << "Target is too crowded, you would be a third wheel! Try another direction!"  ;
               qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

               }
            if (occupation == 3)
               {
               qDebug() << "There is a family at the target position! Try another direction!"  ;
               qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
               }
            else
               {              
               qDebug() << "Target not overbooked yet, you can move there"  ;

               // Move
               this->transformMove( GWSCoordinate( direction[RandIndexX] , direction[RandIndexY] ) );

               // Final position of the agent:                       
               qDebug() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

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

               // Cell occupation constraints
               if (occupation == 1)
                  {
                  qDebug() << this->property("@id").toString()<<", there is a living mate in your position! ";
                  /*
                   *  Reproductive constraints:
                   *   - Internal time threshold
                   *   - Energy threshold
                   */

                  // TO BE CONSIDERED!

                  /*
                   *  Breed!
                   */
                  qDebug() << "************************************************************";
                  qDebug() << "   YAS! You get to breed! Another sheep in the GWSWorld!     ";
                  qDebug() << "************************************************************";
                  this->setProperty("energy" , this->getProperty("energy").toFloat() / 2.0);

                  // Welcome a lamb to the World:
                  SheepAgent* lambAgent = new SheepAgent();
                  GWSExecutionEnvironment::globalInstance()->registerAgent(lambAgent);
                  GWSAgentEnvironment::globalInstance()->registerAgent( lambAgent );
                  lambAgent->setProperty("energy", 10);
                  lambAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );
                  qDebug() << "Lamb position = (" << lambAgent->getCentroid().getX() << ", " << lambAgent->getCentroid().getY() << ")";

                  //  Reproductive constraints:
                  //  - Set internal time counter to 0

                  }
               }

           }


        // Sheep die when:
        if (this->property("energy") < 1)
            {
            this->setProperty(RUNNING_PROP, "FALSE");
            qDebug() << "RIP" << this->property("@id").toString();
            }

}

