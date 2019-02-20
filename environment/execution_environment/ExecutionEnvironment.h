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
    static QString BIRTH_PROP;
    static QString DEATH_PROP;
    static QString STARTED_SIMULATION_TIME;
    static QString STARTED_REAL_TIME;
    static QString ENDED_SIMULATION_TIME;
    static QString ENDED_REAL_TIME;
    static QString CURRENT_TICK_TIME;

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    bool containsAgent( QSharedPointer<GWSAgent> agent ) const;
    int getRunningAgentsAmount() const;
    QList< QSharedPointer<GWSAgent> > getRunningAgents() const;
    template <class T> QList< QSharedPointer<T> > getRunningAgentsByClass( QString class_name ) const;
    bool isRunning() const;
    int getTicksAmount() const;

    // METHODS
    virtual void registerAgent(QSharedPointer<GWSAgent> agent);
    virtual void unregisterAgent(QSharedPointer<GWSAgent> agent);

signals:
    void tickEndedSignal(int executed_tick);
    void runningExecutionSignal();
    void stoppingExecutionSignal();

public slots:
    void run();
    void stop();

private slots:
    void behave();

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

    // Threshold from current_time IN MILLISECONDS
    // Otherwise only minest_tick agent is executed (1 per cycle)
    const quint64 tick_time_window = 9990 * 120;

    // Avoid more than one execution at a time
    mutable QMutex mutex;
};

#endif // GWSEXECUTIONENVIRONMENT_H
