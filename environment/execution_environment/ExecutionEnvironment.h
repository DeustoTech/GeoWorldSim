#ifndef GWSEXECUTIONENVIRONMENT_H
#define GWSEXECUTIONENVIRONMENT_H

#include <QMutex>
#include <QDateTime>
#include <QTimer>
#include <QThread>

#include "../../util/storage/ObjectStorage.h"
#include "../../environment/Environment.h"

namespace geoworldsim {
namespace environment {

class ExecutionEnvironment : public Environment
{
    Q_OBJECT
    friend class App; // App can access

public:
    static ExecutionEnvironment* globalInstance();

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
    bool containsEntity( QSharedPointer<Entity> entity ) const;
    int getRunningAmount() const;
    QList< QSharedPointer<Entity> > getRunning() const;
    template <class T>
    QList< QSharedPointer<T> > getRunning( QString class_name ) const{ return this->environment_entities->getByClass<T>( class_name ); }
    bool isRunning() const;
    int getTicksAmount() const;

    // METHODS
    virtual void registerEntity(QSharedPointer<Entity> entity);
    virtual void unregisterEntity(QSharedPointer<Entity> entity);

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
    ExecutionEnvironment();
    ExecutionEnvironment(ExecutionEnvironment const&);
    ~ExecutionEnvironment();

    // HELPER CLASS
    class ExecutionEnvironmentElement : public QObject {
        public:
            ExecutionEnvironmentElement( QThread* thread , quint64 tick_time_window , uint max_entity_amount_per_tick = -1 ) : QObject() , tick_time_window(tick_time_window) , max_entity_amount_per_tick(max_entity_amount_per_tick) {
                this->moveToThread( thread );
            }
            ~ExecutionEnvironmentElement(){
            }

            // FLAGS
            mutable QReadWriteLock mutext;
            bool is_running = false;
            bool is_busy = false;

            // RUNNING ENTITIES
            QList< QSharedPointer<Entity> > running_storage;
            qint64 min_tick = -1;
            int ticked_entities = 0;
            int ready_entities = 0;
            int busy_entities = 0;
            int future_entities = 0;
            int total_entities = 0;

            // Threshold from current_time IN MILLISECONDS
            // Otherwise only minest_tick entity is executed (1 per cycle)
            const quint64 tick_time_window;
            const uint max_entity_amount_per_tick = -1;

            void behave();
            bool isBusy() const;
    };

    // Cycle amount
    quint64 executed_ticks_amount = 0;
    const uint max_entity_amount_per_tick = -1;

    // Parallel execution chunks
    QMap< QThread* , ExecutionEnvironmentElement* > parallel_executions;

    // ENVIRONMENT MEMORY
    storage::ObjectStorage* environment_entities = Q_NULLPTR;

};

}
}

#endif // GWSEXECUTIONENVIRONMENT_H
