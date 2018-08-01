#ifndef GWSEXECUTIONENVIRONMENT_H
#define GWSEXECUTIONENVIRONMENT_H

#include <QMutex>
#include <QDateTime>
#include <QTimer>

#include "../../util/storage/ObjectStorage.h"
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
    int getRunningAgentsAmount() const;
    QList<GWSAgent*> getRunningAgents() const;
    template <class T> QList<T*> getRunningAgentsByClass( QString class_name ) const;
    bool isRunning() const;
    int getTicksAmount() const;

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
    GWSObjectStorage* running_agents;

    // Timer to schedule ticks
    QTimer* timer = Q_NULLPTR;

    // Cycle amount
    quint64 executed_ticks_amount = 0;

    // Threshold from current_time
    // Otherwise only minest_tick agent is executed (1 per cycle)
    const quint64 tick_time_window = 5*1000;

    // Avoid more than one execution at a time
    mutable QMutex mutex;
};

#endif // GWSEXECUTIONENVIRONMENT_H
