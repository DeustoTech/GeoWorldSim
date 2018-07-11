#ifndef LEASTSQUARESSOLVING
#define LEASTSQUARESSOLVING

#include <QObject>

#include <QVector>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

using namespace Eigen;

class LeastSquaresSolving : public QObject
{
    Q_OBJECT
public:

    LeastSquaresSolving(QObject* parent);
    ~LeastSquaresSolving();

    // METHODS
    QVector<double> leastSquaresSolving(QVector<QVector<double> > matrix, QVector<double> values);
};

#endif // LEASTSQUARESSOLVING

