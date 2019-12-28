#include "NormalDistribution.h"

#include <random>

NormalDistribution::NormalDistribution(QObject *parent) : QObject(parent) {
}

NormalDistribution::~NormalDistribution() {
}

/**********************************************************************
 METHODS
**********************************************************************/

/** Given a mean and a deviation, returns a double deviated
 * from the initial value following a normal function
 * @brief normalDistribution
 * @param mean
 * @param deviation
 * @return
 */
double NormalDistribution::normalDistribution( double mean, double deviation ){
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> distribution(mean , deviation);
    return distribution( generator );
}


