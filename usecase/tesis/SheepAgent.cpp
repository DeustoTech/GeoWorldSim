#include "SheepAgent.h"

SheepAgent::SheepAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "PASO";
}


void SheepAgent::behave(){

    qDebug() << "Soy oveja";

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




    // Get cell_X and cell_y
    qDebug() << "Initial abszise position = " << this->getProperty("cell_x").toString();

    // Move x:
    this->setProperty("cell_x", this->getProperty("cell_x").toInt() + 1);
    qDebug() << "Final abszise position = " << this->getProperty("cell_x").toString();

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
    qDebug() << "Remaining energy after moving and eating = " << this->property("energy").toString();





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


    /* if this->getProperty( SheepAgent::ENERGY_PROPERTY ) <=0
     *      {
     *      SheepAgent::Die(  );
     *      GWSBehaviour::getNext();
     *      }
     * void SheepAgent::Die(  )
            {
            energy = this->getProperty( SheepAgent::ENERGY_PROPERTY );
            this->setProperty( SheepAgent::ENERGY_PROPERTY , energy == 0;);
            GWSBehaviour::getNext();
            }
    */



    this->incrementInternalTime( 5 );
}

