#include "PredatorAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "TerrainAgent.h"


PredatorAgent::PredatorAgent(QObject *parent) : GWSAgent( parent)
{
    qInfo() << "WOLF";
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );
    terrain_agent->enter( this );
}


PredatorAgent::~PredatorAgent(){
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );
    terrain_agent->exit( this );
}


void PredatorAgent::behave()
{
    qInfo() << """""""""""""""""""""""""""""""""""""""""""""""""";
    qInfo() << "                 Wolf                           ";
    qInfo() << "                 ----                           ";

    GWSTimeEnvironment::globalInstance()->incrementAgentInternalTime( this , qrand() % 3 );

    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


    /* Number of agents in the simulation (all types) */
    qInfo() << "Your GWS has " << GWSAgentEnvironment::globalInstance()->getAmount() << "agents.";


    /* Register Terrain Agent so that we can add our sheep
     * to a particular cell of the grid */    
    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId(  TerrainAgent::staticMetaObject.className() , "ThePlayground" );
    TerrainAgent* terrain_agent = dynamic_cast<TerrainAgent*>( agent );


    qInfo() << "Initial energy = " << this->getProperty("energy");

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
        qInfo() << "You choose to stay at the same position. You will eventually die of starvation.";
        this-> setProperty("energy", this->getProperty("energy").toDouble() - this->getProperty("energy").toDouble()/4.);
        qInfo() << "";
       }   
    else  /* And sometimes it will "choose" to move */
       {
       /*
        * Get target cell occupation through AgentGrid methods
        */
       GWSCoordinate centroid = GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid();
       QList<GWSAgent*> targetCellOccupation = terrain_agent->getGridCellValue( centroid.getX() + TargetX, centroid.getY() + TargetY  );

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

                /* Move */
                GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX , TargetY ) );


                /* Moving consumes energy */
                double initialEnergyDouble = this->getProperty("energy").toDouble();
                double moveLosses = initialEnergyDouble / 8.0;
                double finalEnergy = initialEnergyDouble - moveLosses;
                this-> setProperty("energy", finalEnergy);

                /* Feeding supplies energy */
                double foodGains = 50;
                finalEnergy = finalEnergy + foodGains;
                this-> setProperty("energy", finalEnergy);

                /* Unregister the prey */
                qInfo() << "RIP" << targetCellOccupation.at(i)->getProperty("@id").toString();
<<<<<<< HEAD
=======
                terrain_agent->exit(  targetCellOccupation.at(i) );
>>>>>>> 617688a2f83572a101c81ef890500eb5f664c8fa
                QTimer::singleShot( 1000 , targetCellOccupation.at(i) , &GWSAgent::deleteLater );
                return;
                }
            }

        qInfo() << "Target cell PredatorAgent occupation = " << PredatorOccupation;

        if (PredatorOccupation >= 2)
           {
           qInfo() << " You chose to move but target cell is too crowded with wolves! Try another direction or you will eventually die of starvation.";
           qInfo() << "";
           this-> setProperty("energy", this->getProperty("energy").toDouble() - this->getProperty("energy").toDouble()/4.);
           }

        if (PredatorOccupation == 0)
           { 
            qInfo() << "Target is empty! Move!";


            /* Move */
            GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX , TargetY ) );


            /* Moving consumes energy */
            double initialEnergyDouble = this->getProperty("energy").toDouble();
            double moveLosses = initialEnergyDouble / 8.0;
            double finalEnergy = initialEnergyDouble - moveLosses;
            this-> setProperty("energy", finalEnergy);
            }

        if ((PredatorOccupation == 1) && (this-> getProperty("energy").toDouble() >=100.))
           {
           /* Move and breed! */
           qInfo() << this->getProperty("@id").toString()<<", there is a living mate at target! Move and breed!";

           GWSPhysicalEnvironment::globalInstance()->transformMove( this , GWSCoordinate( TargetX , TargetY ) );

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
           cubAgent->setProperty("energy", 200.);
           cubAgent->setProperty("@type", "PredatorAgent");
           GWSPhysicalEnvironment::globalInstance()->transformMove( cubAgent , GWSPhysicalEnvironment::globalInstance()->getGeometry( this )->getCentroid() );
           cubAgent->icon_url = this->icon_url;

           }

           qInfo() << "Energy = " << this->getProperty("energy");

           /* Wolves die when */
           if (this->getProperty("energy") < 1.)
               {
               qInfo() << "RIP" << this->getProperty("@id").toString();
<<<<<<< HEAD
=======
               terrain_agent->exit( this );
>>>>>>> 617688a2f83572a101c81ef890500eb5f664c8fa
               QTimer::singleShot( 0 , this , &GWSAgent::deleteLater );
               }

           }
}












