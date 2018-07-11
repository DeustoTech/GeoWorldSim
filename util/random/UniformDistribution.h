#ifndef UNIFORMDISTRIBUTION_H
#define UNIFORMDISTRIBUTION_H

#include <QObject>

class UniformDistribution : public QObject
{
    Q_OBJECT

public:
    UniformDistribution();

    // METHODS
    static double uniformDistribution();
};

#endif // UNIFORMDISTRIBUTION_H
