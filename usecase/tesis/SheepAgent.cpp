#include "SheepAgent.h"
#include "math.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "PASO";
}


void SheepAgent::behave()
{
        qDebug() << "-------------------------------------------------";
        qDebug() << "Soy la oveja" << this->property("@id").toString();

        // Get cell_X and cell_y
        qDebug() << "Initial position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")";

        // Move randomly through random index generator:
        srand ( time(NULL) ); //initialize the random seed
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = rand() % 3; //generates a random number between 0 and 2
        int RandIndexY = rand() % 3; //generates a random number between 0 and 2

        // Move coordinates either 0, 1 or -1 points
        // Note that the final position should be checked to be within the grid!
        this->setProperty("cell_x", this->getProperty("cell_x").toInt() + direction[RandIndexX]);
        this->setProperty("cell_y", this->getProperty("cell_y").toInt() + direction[RandIndexY]);

        // Final position of the agent:
        qDebug() << "Final position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")";

        // Get initial energy
        float initialEnergyFloat = this->getProperty("energy").toFloat();
        //float nearest = roundf(val * 100) / 100;
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

        // Generate a list with all the sheeps in the GWS world:
        QList<GWSAgent*> sheeps = GWSAgentEnvironment::globalInstance()->getByClass( SheepAgent::staticMetaObject.className() );

        // This list allows us to loop over all the existing sheeps and to check whether they share
        // the same position!

        /* TOMORROW HERE!
        if ((this->property("cell_x") == theotherAgent->property("cell_x"))
           &&
           (this->property("cell_y") == theotherAgent->property("cell_y"))
           &&
           (theotherAgent->property("@type") == "SheepAgent")
           &&
           (theotherAgent->property("running") == "true"))
           // And we should eventually add the condition of "reproduce only after x ticks"
                {
                this->setProperty("energy" , this->getProperty("energy") / 2.0);
                theotherAgent->setProperty("energy" , theotherAgent->getProperty("energy") / 2.0);
                SheepAgent* lambAgent = new SheepAgent();
                GWSExecutionEnvironment::globalInstance()->registerAgent( lambAgent);
                lambAgent->setProperty("energy", 100);
                lambAgent->setProperty("cell_x", this->property("cell_x"));
                lambAgent->setProperty("cell_y", this->property("cell_y"));
                }
        */


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


// Reproduction:
/*
 1 - CHECK CELL OCCUPATION:

    // check if the main agent's and the second agent's position is the same:
    // but how can I know within the sheep class whether there is another agent in the cell?
    // Perhaps I need to write these lines in main.cpp
    1 ) if ((this->property("cell_x") == theotherAgent->property("cell_x"))
        &&
        (this->property("cell_y") == theotherAgent->property("cell_y"))
        {
        CHECK IF THE AGENT IS SHEEP through 2)
        }

 2 - CHECK IF OCCUPIED BY SHEEP:
    // And insert this as a method of the SheepAgent class:
    2 )  if  ((theotherAgent->property("@type") == "SheepAgent")
              &&
              (theotherAgent->property("running") == "true"))
                {
                this->setProperty("energy" , this->getProperty("energy") / 2.0);
                theotherAgent->setProperty("energy" , theotherAgent->getProperty("energy") / 2.0);
                SheepAgent* lambAgent = new SheepAgent();
                GWSExecutionEnvironment::globalInstance()->registerAgent( agent3 );
                lambAgent->setProperty("energy", 100);
                lambAgent->setProperty("cell_x", this->property("cell_x"));
                lambAgent->setProperty("cell_y", this->property("cell_y"));
                }


    // This is another possibility that involves manually inputting the SheepAgents we register in our world
    // into an array and checking whether at some point they are in the same position.
    // The drawback is that this should be within the main.cpp file, because we need to hardcode
    // the vector of agents...

    GWSAgent* SheepFlock[3] = {agent1, agent2, agent3};
    for (int i = 0; i <= 3; i++)
        {
        for (int j = 0; j <= 3 ; j++ )
            {
            if (
               (SheepFlock[i]->property("cell_x") == SheepFlock[j]->property("cell_x"))
                &&
               (SheepFlock[i]->property("cell_y") == SheepFlock[j]->property("cell_y"))
                &&
               (i != j)
               )
               {
               SheepAgent* lambAgent = new SheepAgent();
               lambAgent->setProperty("energy", 100);
               lambAgent->setProperty("cell_x", this->property("cell_x"));
               lambAgent->setProperty("cell_y", this->property("cell_y"));
               this->setProperty("energy", this->property("energy").toFloat() / 2.0)
               }
           else
               {
               qDebug() << "Not overbooked yet!";
               }
            }
}
*/


