#include "PredatorAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "TerrainAgent.h"


PredatorAgent::PredatorAgent(QObject *parent) : GWSAgent( parent)
{
qInfo() << "WOLF";
}


PredatorAgent::~PredatorAgent(){
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );
    terrain_agent->removeGridCellValue( this->getCentroid().getX() , this->getCentroid().getY() , this );
}


void PredatorAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Wolf                           ";
    qInfo() << "                 ----                           ";


    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Number of agents in the simulation (all types) */
    qInfo() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";


    /* Register Terrain Agent so that we can add our sheep
     * to a particular cell of the grid */    
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );


    /* Print wolf's cell_X and cell_y and original cell occupation */
    qInfo() << "I am" << this->getProperty("@id").toString();
    qInfo() << "Original cell = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";
    qInfo() << "Occupation of original cell = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());


    /* Move randomly through random index generator */    
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
    int RandIndexY = qrand() % 3; //generates a random number between 0 and 2


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
       //qInfo()  << "Target cell occupation = " << targetCellOccupation;
       int PredatorOccupation = 0;

       /*Number of sheep and wolves in target cell */
       for (int i = 0; i < targetCellOccupation.size(); ++i)
            {
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "PredatorAgent")
                {
                PredatorOccupation += 1;
                }
            if (targetCellOccupation.at(i)->getProperty("@type").toString() == "SheepAgent")
                {                
                qInfo() << "Found sheep at target! Move and eat!";

                /* Notify the grid that the wolf is leaving */
                terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

                /* Move */
                this->transformMove( GWSCoordinate( TargetX , TargetY ) );

                /* Final position of the agent */
                qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

                /* Notify the grid of the wolf's new position */
                terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
                qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

                /* Moving consumes energy */
                double initialEnergyDouble = this->getProperty("energy").toDouble();
                double moveLosses = initialEnergyDouble / 8.0;
                double finalEnergy = initialEnergyDouble - moveLosses;
                this-> setProperty("energy", finalEnergy);

                /* Feeding supplies energy */
                double foodGains = finalEnergy / 2.0;
                finalEnergy = finalEnergy + foodGains;
                this-> setProperty("energy", finalEnergy);

                /* Unregister the prey */                
                qInfo() << "RIP" << targetCellOccupation.at(i)->getProperty("@id").toString();
                terrain_agent->removeGridCellValue(targetCellOccupation.at(i)->getCentroid().getX(), targetCellOccupation.at(i)->getCentroid().getY(), targetCellOccupation.at(i));
                QTimer::singleShot( 0 , targetCellOccupation.at(i) , &GWSAgent::deleteLater );
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
            /* Move! */
            qInfo() << "Target is empty! Move!";

            /* Notify the grid that the wolf is leaving */
            terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

            /* Move */
            this->transformMove( GWSCoordinate( TargetX , TargetY ) );

            /* Final position of the agent */
            qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

            /* Notify the grid of the wolf's new position */
            terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
            qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

            /* Moving consumes energy */
            double initialEnergyDouble = this->getProperty("energy").toDouble();
            double moveLosses = initialEnergyDouble / 8.0;
            double finalEnergy = initialEnergyDouble - moveLosses;
            this-> setProperty("energy", finalEnergy);
            }
        if (PredatorOccupation == 1)
           {
           /* Move and breed! */
           qInfo() << this->getProperty("@id").toString()<<", there is a living mate at target! Move and breed!";

           /* Notify the grid that the wolf is leaving */
           terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);

           /* Move */
           this->transformMove( GWSCoordinate( TargetX , TargetY ) );

           /* Final position of the agent */
           qInfo() << "Final position = (" << this->getCentroid().getX() << ", " << this->getCentroid().getY() << ")";

           /* Notify the grid of the wolf's new position */
           terrain_agent->addGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
           qInfo() << "Final cell occupation = " << terrain_agent->getGridCellValue(this->getCentroid().getX(), this->getCentroid().getY());

           /* Moving consumes energy */
           double initialEnergyDouble = this->getProperty("energy").toDouble();
           double moveLosses = initialEnergyDouble / 8.0;
           double finalEnergy = initialEnergyDouble - moveLosses;
           this-> setProperty("energy", finalEnergy);

           /* Breeding consumes energy */
           this->setProperty("energy" , this->getProperty("energy").toDouble() / 2.0);

           /* Add a cub to the World */
           PredatorAgent* cubAgent = new PredatorAgent();
           GWSEnvironment::globalInstance()->registerAgent( cubAgent );
           GWSExecutionEnvironment::globalInstance()->registerAgent( cubAgent );


           /* Set cub's properties */
           cubAgent->setProperty("energy", 10);
           cubAgent->setProperty("@type", "PredatorAgent");
           cubAgent->transformMove( GWSCoordinate( this->getCentroid().getX() , this->getCentroid().getY() ) );
           cubAgent->setProperty("style", "icon_url\" : \"https://image.flaticon.com/icons/svg/616/616457.svg" );
           /* Notify the grid of new cub's position */
           terrain_agent->addGridCellValue(cubAgent->getCentroid().getX(), cubAgent->getCentroid().getY(), cubAgent);
           }

           /* Wolves die when */
           if (this->getProperty("energy") < 1.)
               {
               qInfo() << "RIP" << this->getProperty("@id").toString();
               terrain_agent->removeGridCellValue(this->getCentroid().getX(), this->getCentroid().getY(), this);
               QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
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









