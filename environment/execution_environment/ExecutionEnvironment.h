#ifndef GWSEXECUTIONENVIRONMENT_H
#define GWSEXECUTIONENVIRONMENT_H

#include <QMutex>
#include <QDateTime>
#include <QTimer>

#include "../../environment/Environment.h"

class GWSExecutionEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSApp; // App can access

public:
    static GWSExecutionEnvironment* globalInstance();

    // PROPERTIES
    static QString RUNNING_PROP;

    // EXPORTERS
    virtual QJsonObject serializeMini();
    virtual QJsonObject serialize();

    // GETTERS
    int getRunningAgents() const;
    bool isRunning() const;

    // METHODS
    virtual void registerAgent(GWSAgent* agent);
    virtual void unregisterAgent(GWSAgent* agent);

signals:
    void tickEndedSignal(int executed_tick);
    void runningExecutionSignal();
    void stoppingExecutionSignal();

public slots:
    void run();
    void stop();

private slots:
    void tick();

private:
    GWSExecutionEnvironment();
    GWSExecutionEnvironment(GWSExecutionEnvironment const&);
    ~GWSExecutionEnvironment();

    // RUNNING AGENTS
    QList<GWSAgent*> running_agents;

    // Timer to schedule ticks
    QTimer* timer = Q_NULLPTR;

    // Cycle amount
    quint64 executed_ticks_amount = 0;

    // Threshold from current_time
    // Otherwise only minest_tick agent is executed (1 per cycle)
    const quint64 min_tick_threshold = 1*1000;

    // Avoid more than one execution at a time
    mutable QMutex mutex;
};

#endif // GWSEXECUTIONENVIRONMENT_H
