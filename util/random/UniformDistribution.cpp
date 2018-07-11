#include "UniformDistribution.h"

UniformDistribution::UniformDistribution() : QObject(){
}

/** Generate value from uniform distribution
 * @brief uniformDistribution
 * @return
 */
double UniformDistribution::uniformDistribution(){
    std::random_device rd;
    std::mt19937 generator( rd() );
    std::uniform_real_distribution<double> urd = std::uniform_real_distribution<double> (0.0,1.0);
    return urd( generator );
}
