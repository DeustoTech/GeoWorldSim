#ifndef CONFUSIONMATRIX_H
#define CONFUSIONMATRIX_H

#include <QObject>
#include <QMap>

class ConfusionMatrix : public QObject
{
    Q_OBJECT

public:
    explicit ConfusionMatrix(QObject *parent = 0);
    ConfusionMatrix(const ConfusionMatrix&);
    ~ConfusionMatrix();

    // GETTERS
    double getValue( QString x , QString y ) const;
    double getSum( QString x ) const;
    QMap< QString , double > getDiagonal() const;
    QStringList getXNames() const;
    QStringList getYNames( QString x ) const;

    // SETTERS
    void sumValue( QString x , QString y , double v );

private:
    QMap< QString , QMap < QString , double > > matrix;

};

#endif // CONFUSIONMATRIX_H
