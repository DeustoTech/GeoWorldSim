#ifndef ROULETEWHEEL_H
#define ROULETEWHEEL_H

#include <QObject>
#include <QList>

#include "utils/random_generators/UniformDistribution.h"

class RouletteWheel : public QObject
{
    Q_OBJECT

public:

    RouletteWheel(QObject* parent = 0);
    ~RouletteWheel();

    // GETTERS
    bool isEmpty();

    // SETTERS
    void addElement( QObject* obj, double frequency );

    // METHODS
    QObject* getElement();
    QObject* getElement( double frequency );

private:
    QList<QObject*> roulette;   // pair of object and frequency
    QList<double> ordered_frequencies; // frequencies ordered in ascending order
    double sum_frequencies;
};

#endif // ROULETEWHEEL_H
