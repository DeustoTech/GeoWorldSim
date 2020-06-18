#ifndef UNIFORMDISTRIBUTION_H
#define UNIFORMDISTRIBUTION_H

#include <QObject>

namespace geoworldsim {
namespace random {

class UniformDistribution : public QObject
{
    Q_OBJECT

public:
    UniformDistribution();

    // METHODS
    static double uniformDistribution();
};

}
}

#endif // UNIFORMDISTRIBUTION_H
