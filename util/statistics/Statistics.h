#ifndef STATISTICS_H
#define STATISTICS_H

#include <QObject>
#include <QList>

class Statistics : public QObject
{
    Q_OBJECT

public:
    explicit Statistics(QObject *parent = 0);

    // GETTERS
    static double mean(QList<double> values);
    static double std(QList<double> values);
    static double variance(QList<double> values);
    static double max(QList<double> values);
    static double min(QList<double> values);

};

#endif // STATISTICS_H
