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
    template <class T> QList< QSharedPointer<T> > getRunning( QString class_name ) const{
        QList< QSharedPointer< T > > l;
        foreach (GWSExecutionEnvironmentElement* p , this->parallel_executions ) {
            l.append( p->running_storage->getByClass<T>( class_name ) );
        }
        return l;
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

    // HELPER CLASS
    class GWSExecutionEnvironmentElement : public QObject {
        public:
            GWSExecutionEnvironmentElement( QThread* thread , quint64 tick_time_window , uint max_entity_amount_per_tick = -1 ) : QObject() , tick_time_window(tick_time_window) , max_entity_amount_per_tick(max_entity_amount_per_tick) {
                this->moveToThread( thread );
                this->running_storage = new GWSObjectStorage();
            }
            ~GWSExecutionEnvironmentElement(){
                this->running_storage->deleteLater();
            }

            // FLAGS
            QReadWriteLock mutext;
            bool is_running = false;
            bool is_busy = false;

            // RUNNING ENTITIES
            GWSObjectStorage* running_storage;
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
    };

    // Cycle amount
    quint64 executed_ticks_amount = 0;
    const uint max_entity_amount_per_tick = -1;

    // Parallel execution chunks
    QMap< QThread* , GWSExecutionEnvironmentElement* > parallel_executions;

};

#endif // GWSEXECUTIONENVIRONMENT_H
