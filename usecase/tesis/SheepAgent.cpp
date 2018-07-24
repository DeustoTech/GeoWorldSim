#include "SheepAgent.h"
#include "math.h"
SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "PASO";
}


void SheepAgent::behave()
{
    //do {
        qDebug() << "Soy oveja";

        // Get cell_X and cell_y
        qDebug() << "Initial position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")" << endl;

        // Move randomly through random index generator:
        srand ( time(NULL) ); //initialize the random seed
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = rand() % 3; //generates a random number between 0 and 2
        int RandIndexY = rand() % 3; //generates a random number between 0 and 2

        // Move coordinates either 0, 1 or -1 points
        this->setProperty("cell_x", this->getProperty("cell_x").toInt() + direction[RandIndexX]);
        this->setProperty("cell_y", this->getProperty("cell_y").toInt() + direction[RandIndexY]);

        // Final position of the agent:
        qDebug() << "Final position = (" << this->getProperty("cell_x").toString() << ", " << this->getProperty("cell_y").toString() << ")" << endl;

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
       // }

    //while (this->property("energy").toFloat() >= 0.01);

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



/*
 *  The behaviours we want from sheep:
 *  1. Move
 *      1.1 Get position
 *      1.2 Change position to new position
 *      1.3 Lose energy from moving
 *      1.4 If energy < 0, die
 *      1.5 Go to next
 *  2. Eat
 *      2.1 Increment energy from nourishment
 *      2.2 Go to next
 *  3. Reproduce
 *      3.1 Only if another sheep in new position
 *      3.2 Only after a given number of moves
 *      3.3 Lose half of the energy
 *      3.4 If energy < 0, die
 *      3.5 Go to next
 */

/* void SheepAgent::Move()
        {
        energy = this->getProperty( GWSAgent::ENERGY_PROPERTY );
        this->setProperty( GWSAgent::ENERGY_PROPERTY , energy -= energy / 4.0);
        GWSBehaviour::getNext();
        }
*/
