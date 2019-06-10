#ifndef GWSEXECUTIONENVIRONMENT_H
#define GWSEXECUTIONENVIRONMENT_H

#include <QMutex>
#include <QDateTime>
#include <QTimer>

#include "../../util/storage/ObjectStorage.h"
#include "../../environment/Environment.h"

using namespace QtConcurrent;

class GWSExecutionEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSApp; // App can access

public:
    static GWSExecutionEnvironment* globalInstance();

    // PROPERTIES
    static QString ENTITY_BIRTH_PROP;
    static QString ENTITY_DEATH_PROP;
    static QString ENTITY_RUNNING_PROP;
    static QString STARTED_SIMULATION_TIME;
    static QString STARTED_REAL_TIME;
    static QString ENDED_SIMULATION_TIME;
    static QString ENDED_REAL_TIME;
    static QString CURRENT_TICK_TIME;
    static QString WAIT_FOR_ME_PROP;

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    bool containsEntity( QSharedPointer<GWSEntity> entity ) const;
    int getRunningAmount() const;
    QList< QSharedPointer<GWSEntity> > getRunning() const;
    template <class T> QList< QSharedPointer<T> > getRunningByClass( QString class_name ) const{
        return this->running_entities->getByClass<T>( class_name );
    }
    bool isRunning() const;
    int getTicksAmount() const;

    // METHODS
    virtual void registerEntity(QSharedPointer<GWSEntity> entity);
    virtual void unregisterEntity(QSharedPointer<GWSEntity> entity);

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

    // RUNNING ENTITIES
    GWSObjectStorage* running_entities;

    // Cycle amount
    quint64 executed_ticks_amount = 0;

    // Threshold from current_time IN MILLISECONDS
    // Otherwise only minest_tick entity is executed (1 per cycle)
    qint64 last_tick_with_entities = 0;
    const quint64 tick_time_window;
    const uint max_entity_amount_per_tick;

    // Avoid more than one execution at a time
    mutable QMutex mutex;
};

#endif // GWSEXECUTIONENVIRONMENT_H
