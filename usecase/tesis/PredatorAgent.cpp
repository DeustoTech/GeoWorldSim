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
    qInfo() << "--------------- Wolf ---------------------------";


    /*
     *  WOLF BEHAVIOUR:
     *  1 - MOVE -> LOSE ENERGY FROM MOVING
     *  2 - IF SHEEP IN FINAL CELL -> EAT -> GAIN ENERGY
     *  3 - IF WOLF PRESENT IN FINAL CELL -> REPRODUCE -> LOSE ENERGY
     *  4 - DIE IF NEGATIVE ENERGY
     */

    // Send information to website
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Register Terrain Agent so that we can add our sheep
     * to a particular cell of the grid*/
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );


    /*
     *  Generate a list with all the wolves in the GWS world.
     *  This list allows us to loop over all the existing wolves
     *  and to check whether they share the same position for breeding purposes!
     */
    QList<GWSAgent*> wolves = GWSAgentEnvironment::globalInstance()->getByClass( PredatorAgent::staticMetaObject.className() );
    qDebug() << "Number of running PredatorAgents in the field = "<< wolves.size() ;


    // Get cell_X and cell_y
    qDebug() << "I am" << this->property("@id").toString();
    qDebug() << "Initial position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

    // Move randomly through random index generator. We get more realistic random behaviour if the seed is not initialized:
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = rand() % 3; //generates a random number between 0 and 2
    int RandIndexY = rand() % 3; //generates a random number between 0 and 2

    // Move coordinates either 0, 1 or -1 points
    // TODO: final position has to be within the grid!
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];
    int PredatorOccupation = 0;
    int SheepOccupation = 0;
    int occupation = 0;

    // The wolf will stay on same position:
    if ((TargetX == 0) && (TargetY == 0))
       {
        /*
         *  We assume a wolf will only nourish when it moves to a different position.
         *  Same applies to breeding.
         */
        qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";
        qInfo() << "You choose to stay at the same position. No energy lost, nor gained, no breeding.";
        qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
       }

    else
       {
       qDebug() << "Target movement = ("<< TargetX << "," << TargetY<< ")";

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

        qDebug() << "Target cell PredatorAgent occupation = " << PredatorOccupation;

        /*
         * Get target cell sheep occupation through AgentGrid methods
         */

        QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue(this->getCentroid().getX() + TargetX, this->getCentroid().getY() + TargetY  );

        // Number of sheep in target cell:
        for (int i = 0; i < targetCellOccupation.size(); ++i)
            {
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
                {
                occupation += 1;
                }
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
                {
                SheepOccupation += 1;
                }
            }

        qDebug() << "Target cell PredatorAgent occupation = " << occupation;
        qDebug() << "Target cell SheepAgent occupation = " << SheepOccupation;

        /*
         * Modify behaviour based on target cell occupation
         * If there is another wolf, move and breed.
         */
        if (PredatorOccupation >= 2)
           {
           qDebug() << "Breeding going on here! Try another direction!"  ;
           qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
           }
        if (PredatorOccupation == 0)
           {
            /*
            *  Move!
            */
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
            /*
             *  Move and Breed!
             */
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

            /*
             *  Reproductive constraints:
             *   - Internal time threshold
             *   - Energy threshold
             */

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



        }

    // Wolves die when:
    if (this->property("energy") < 1.)
        {
        this->setProperty(RUNNING_PROP, "FALSE");
        qInfo() << "****************************************";
        qInfo() << "RIP" << this->property("@id").toString();
        qInfo() << "****************************************";
        }
}
