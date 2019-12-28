#ifndef NUMBERSORTER_H
#define NUMBERSORTER_H

#include <QObject>
#include <QVector>

class NumberSorter : public QObject
{
    Q_OBJECT

public:
    explicit NumberSorter(QObject *parent = 0);

    static QVector<double> sortAscending(QVector<double> unsorted_list);

};

#endif // NUMBERSORTER_H
