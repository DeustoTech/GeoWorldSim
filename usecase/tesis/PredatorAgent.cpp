#include "PredatorAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "TerrainAgent.h"


PredatorAgent::PredatorAgent(QObject *parent) : GWSAgent( parent)
{
qInfo() << "WOLF";
}



void PredatorAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Wolf                           ";
    qInfo() << "                 ----                           ";

    // Send information to website
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );

    // Number of agents in the simulation (all types).
    // NOTE: The following is equivalent to GWSExecutionEnvironment::globalInstance()->getRunningAgentsAmount()
    qDebug() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";

    /* Generate a list with all the wolves in the GWS world.
     * This list allows us to loop over all the existing wolves
     * and to check whether they share the same position for breeding purposes!*/
    QList<GWSAgent*> wolves = GWSAgentEnvironment::globalInstance()->getByClass( PredatorAgent::staticMetaObject.className() );
    qDebug() << "Number of running PredatorAgents in the field = "<< wolves.size() ;

    /* Register Terrain Agent so that we can add our sheep
     * to a particular cell of the grid */
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );

    /* Get wolf's cell_X and cell_y and original cell occupation */
    qInfo() << "I am" << this->property("@id").toString();
    qInfo() << "Original cell = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
    qInfo() << "Occupation of original cell = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

    /* Move randomly through random index generator */
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = rand() % 3; //generates a random number between 0 and 2
    int RandIndexY = rand() % 3; //generates a random number between 0 and 2

    /* Move coordinates according to random index */
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];

    /* We assume a wolf will only nourish when it moves to a different position.
     * Same applies to breeding. */

    qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";

    if ((TargetX == 0) && (TargetY == 0))  /* Sometimes the wolf will "choose" to stay on same position */
       {
        qInfo() << "";
        qInfo() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
        qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
        qInfo() << "";
       }   
    else  /* And sometimes it will "choose" to move */
       {
       /*
        * Get target cell occupation through AgentGrid methods
        */
       QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue(this->getCentroid().getX() + TargetX, this->getCentroid().getY() + TargetY  );
       qInfo()  << "Target cell occupation = " << targetCellOccupation;
       int PredatorOccupation = 0;

       /*
        * Modify behaviour based on target cell occupation
        * Wolves move if
        *      1 - The target cell is empty of sheep and wolf
        *      2 - If there is a single wolf at target cell ->  They get to BREED!
        *      3 - If there is at least one sheep at target cell -> They get to EAT!
        * Wolves stay if
        *      4 - Random movement is (0, 0)
        *      5 - There are 2 or more wolves at target cell
        */

       // Number of sheep and wolves in target cell:
       for (int i = 0; i < targetCellOccupation.size(); ++i)
            {
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
                {
                PredatorOccupation += 1;
                }
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
                {

                qInfo() << "Found a sheep at target!";
                /*
                 *  Move and eat as soon as you find a sheep at target!
                 */
                // Notify the grid that the wolf is leaving:
                terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

                // Move
                this->transformMove( GWSCoordinate( TargetX , TargetY ) );

                // Final position of the agent:
                qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

                // Notify the grid of the wolf's new position:
                terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
                qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

                // Get initial energy
                float initialEnergyFloat = this->getProperty("energy").toFloat();

                // Moving consumes energy:
                float moveLosses = initialEnergyFloat / 8.0;

                // The final energy after moving:
                float finalEnergy = initialEnergyFloat - moveLosses;
                qDebug() << "Energy after moving = " << finalEnergy;

                this-> setProperty("energy", finalEnergy);

                qDebug() << "*************************";
                qDebug() << "   YAS! You get to eat! ";
                qDebug() << "*************************";

                // Eating supplies energy:
                float foodGains = finalEnergy / 2.0;

                // The final energy after eating:
                finalEnergy = finalEnergy + foodGains;
                qInfo() << "Energy after eating = " << finalEnergy;

                this-> setProperty("energy", finalEnergy);

                /* Unregister the prey */
                GWSExecutionEnvironment::globalInstance()->unregisterAgent( targetCellOccupation.at(i) );
                terrain_agent->removeGridCellValue(targetCellOccupation.at(i)->getCentroid().getX(), targetCellOccupation.at(i)->getCentroid().getY(), targetCellOccupation.at(i));
                qInfo() << "****************************************";
                qInfo() << "RIP" << targetCellOccupation.at(i)->property("@id").toString();
                qInfo() << "****************************************";
                qDebug() << "Target cell occupation after eating = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());
                return;
                }
            }

        qInfo() << "Target cell PredatorAgent occupation = " << PredatorOccupation;

        if (PredatorOccupation >= 2)
           {
           qInfo() << " You chose to move but target cell is too crowded with wolves! Try another direction!";
           qInfo() << "";
           }
        if (PredatorOccupation == 0)
           {
            qInfo() << "Target is empty!";
            /*
             *  Move!
             */
            // Notify the grid that the wolf is leaving:
            terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

            // Move
            this->transformMove( GWSCoordinate( TargetX , TargetY ) );

            // Final position of the agent:
            qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

            // Notify the grid of the wolf's new position:
            terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
            qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

            // Get initial energy
            float initialEnergyFloat = this->getProperty("energy").toFloat();

            // Moving consumes energy:
            float moveLosses = initialEnergyFloat / 8.0;

            // The final energy after moving:
            float finalEnergy = initialEnergyFloat - moveLosses;
            qDebug() << "Energy after moving = " << finalEnergy;

            this-> setProperty("energy", finalEnergy);
            }
        if (PredatorOccupation == 1)
           {
           /*
            *  Move and breed!
            */
           qDebug() << this->property("@id").toString()<<", there is a living mate at target!";

           // Notify the grid that the wolf is leaving:
           terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

           // Move
           this->transformMove( GWSCoordinate( TargetX , TargetY ) );

           // Final position of the agent:
           qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

           // Notify the grid of the wolf's new position:
           terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
           qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

           // Get initial energy
           float initialEnergyFloat = this->getProperty("energy").toFloat();

           // Moving consumes energy:
           float moveLosses = initialEnergyFloat / 8.0;

           // The final energy after moving:
           float finalEnergy = initialEnergyFloat - moveLosses;
           qDebug() << "Energy after moving = " << finalEnergy;

           this-> setProperty("energy", finalEnergy);

           /*
            *  Reproductive constraints // TO BE CONSIDERED!
            *   - Internal time threshold
            *   - Energy threshold
            */

           qDebug() << "************************************************************";
           qDebug() << "   YAS! You get to breed! Another wolf in the GWSWorld!     ";
           qDebug() << "************************************************************";
           this->setProperty("energy" , this->getProperty("energy").toFloat() / 2.0);
           qDebug() << "Energy after breeding = " << this->getProperty("energy").toFloat();

            // Welcome a cub to the World:
            PredatorAgent* cubAgent = new PredatorAgent();
            GWSExecutionEnvironment::globalInstance()->registerAgent(cubAgent);
            GWSAgentEnvironment::globalInstance()->registerAgent( cubAgent );
            cubAgent->setProperty("energy", 10);
            cubAgent->setProperty("@type", "PredatorAgent");
            cubAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );
            qInfo() << "Cub position = (" << cubAgent->getCentroid().getX() << ", " << cubAgent->getCentroid().getY() << ")";

            // Notify the grid of new cub's position:
            terrain_agent->addGridCellValue(cubAgent->getCentroid().getX(), cubAgent->getCentroid().getY(), cubAgent);
            qDebug() << "Target cell occupation after breeding = " << terrain_agent->getGridCellValue(cubAgent->getCentroid().getX(), cubAgent->getCentroid().getY());

            //  Reproductive constraints:
            //  - Set internal time counter to 0
            }

           // Wolves die when:
           if (this->property("energy") < 1.)
               {
               GWSExecutionEnvironment::globalInstance()->unregisterAgent( this );
               terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
               qInfo() << "****************************************";
               qInfo() << "RIP" << this->property("@id").toString();
               qInfo() << "****************************************";
               }

           }
}





/*
// Loop to see how many wolves are at the target point:
for (int i = 0; i < wolves.size(); ++i)
     {
     if (
          // There is a PredatorAgent that shares your X coord
          (wolves.at(i)->getCentroid().getX() == this->getCentroid().getX() + TargetX)
          && // ... and your Y coord
          (wolves.at(i)->getCentroid().getY() == this->getCentroid().getY() + TargetY)
          && // ... and it is not yourself
          (wolves.at(i)->property("@id") != this->property("@id"))
          && // ... and it is not dead
          (wolves.at(i)->property("running") == "true"))
             {
             // Increase the occupation
             PredatorOccupation += 1;
             }
     }

 qDebug() << "Target cell PredatorAgent occupation = " << PredatorOccupation;*/









/*
 * Modify behaviour based on target cell occupation
 * If there is another wolf, move and breed.

if (PredatorOccupation >= 2)
   {
   qInfo() << "Breeding going on here! Try another direction!"  ;
   qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
   }
if (PredatorOccupation == 0)
   {

    //  Move!

    qDebug() << "Target not overbooked yet, you can move there";

    // Move
    this->transformMove( GWSCoordinate( direction[RandIndexX] , direction[RandIndexY] ) );

    // Get initial energy
    float initialEnergyFloat = this->getProperty("energy").toFloat();

    // Moving consumes energy:
    float moveLosses = initialEnergyFloat / 8.0;

    // The final energy after moving:
    float finalEnergy = initialEnergyFloat - moveLosses;
    qDebug() << "Energy after moving = " << finalEnergy;

    this-> setProperty("energy", finalEnergy);

    // Final position of the agent:
    qDebug() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

    }
if (PredatorOccupation == 1)
   {

    //  Move and Breed!

    qDebug() << this->property("@id").toString()<<", there is a living mate at target! Move there!";

    // Move
    this->transformMove( GWSCoordinate( direction[RandIndexX] , direction[RandIndexY] ) );

    // Get initial energy
    float initialEnergyFloat = this->getProperty("energy").toFloat();

    // Moving consumes energy:
    float moveLosses = initialEnergyFloat / 8.0;

    // The final energy after moving:
    float finalEnergy = initialEnergyFloat - moveLosses;
    qDebug() << "Energy after moving = " << finalEnergy;

    this-> setProperty("energy", finalEnergy);

    // Final position of the agent:
    qDebug() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";


     //  Reproductive constraints:
     //   - Internal time threshold
     //   - Energy threshold
     //

    // TO BE CONSIDERED!


    qDebug() << "************************************************************";
    qDebug() << "   YAS! You get to breed! Another wolf in the GWSWorld!     ";
    qDebug() << "************************************************************";
    this->setProperty("energy" , this->getProperty("energy").toFloat() / 2.0);
    qDebug() << "Energy after breeding = " << this->getProperty("energy").toFloat();

    // Welcome a cub to the World:
    PredatorAgent* cubAgent = new PredatorAgent();
    GWSExecutionEnvironment::globalInstance()->registerAgent(cubAgent);
    GWSAgentEnvironment::globalInstance()->registerAgent( cubAgent );
    cubAgent->setProperty("energy", 10);
    cubAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );
    qInfo() << "Cub position = (" << cubAgent->getCentroid().getX() << ", " << cubAgent->getCentroid().getY() << ")";

   //  Reproductive constraints:
   //  - Set internal time counter to 0
   }



}*/

