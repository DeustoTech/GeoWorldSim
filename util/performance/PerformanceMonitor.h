#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QObject>
#include <QStringList>
#include <QElapsedTimer>
#include <QTimer>

class GWSPerformanceMonitor : public QObject
{
    Q_OBJECT

public:

    GWSPerformanceMonitor();

    // GETTERS

public slots:
    void startWatchingPerformance();
    void stopWatchingPerformance();

    static QStringList getSystemInfo();

private slots:
    void storeCurrentPerformance();

private:

    // METHODS
    QStringList getUsage();

    // Attributes
    QElapsedTimer elapsed_time;
    QTimer* timer = Q_NULLPTR;

    // Stored variables
    int max_threads_count;
    int max_running_agents_count;

};

#endif // PERFORMANCEMONITOR_H
