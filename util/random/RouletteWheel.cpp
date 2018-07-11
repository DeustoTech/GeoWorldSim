#include "RouletteWheel.h"

#include <QTime>
#include <QtMath>
#include <cmath>

RouletteWheel::RouletteWheel(QObject *parent) : QObject(parent) {
    this->sum_frequencies = 0;
    this->ordered_frequencies.empty();
    this->roulette.empty();
}

RouletteWheel::~RouletteWheel() {
}


/**********************************************************************
 GETTERS
**********************************************************************/

bool RouletteWheel::isEmpty(){
    return this->roulette.isEmpty();
}

/**********************************************************************
 SETTERS
**********************************************************************/

/**
 * Adds element to QMap of <Object, frequencies>
 * @brief RouletteWheel::addElement
 * @param frequency
 * @param obj
 */
void RouletteWheel::addElement( QObject* obj, double frequency ){
    if( frequency == NAN || frequency == INFINITY || frequency != frequency ){
        return;
    }

    if ( this->ordered_frequencies.isEmpty() || this->ordered_frequencies.last() <= frequency ){
        this->ordered_frequencies.append( frequency );
        this->roulette.append( obj );

    } else {

        // Insert frequency in ascending order
        for ( int i = 0; i < this->ordered_frequencies.size(); i++ ){

            if( frequency <= this->ordered_frequencies.at(i) ){
                this->ordered_frequencies.insert( i , frequency );
                this->roulette.insert( i , obj );
                break;
            }
        }
    }
    this->sum_frequencies += frequency;
}


/**********************************************************************
 METHODS
**********************************************************************/

/**
 * Generates a random value to get random element
 * based on roulette wheel selection
 * @brief RouletteWheel::getElement
 * @return
 */
QObject* RouletteWheel::getElement(){
    double value = UniformDistribution::uniformDistribution(); // Returns from 0 to 1
    return getElement( value );
}

/**
 * Retrieves the element which frequency falls within the value
 * passed as parameter
 * @brief RouletteWheel::getElement
 * @param value
 * @return
 */
QObject* RouletteWheel::getElement( double value ){
    double accumulative = 0;
    value = value * this->sum_frequencies;

    for ( int i = 0; i < this->ordered_frequencies.size(); i++ ){
        accumulative += this->ordered_frequencies.at( i );
        if( accumulative >= value ){
            return this->roulette.at( i );
        }
    }
    return 0;
}
