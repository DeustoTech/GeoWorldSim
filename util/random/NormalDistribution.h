#ifndef NORMALDISTRIBUTION
#define NORMALDISTRIBUTION

#include <QObject>

class NormalDistribution : public QObject
{
    Q_OBJECT
public:

    NormalDistribution(QObject* parent);
    ~NormalDistribution();

    // METHODS
    double normalDistribution(double mean, double deviation);
};

#endif // NORMALDISTRIBUTION

