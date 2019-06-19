#ifndef GWSPARALLELISMCONTROLLER_H
#define GWSPARALLELISMCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QThread>

class GWSParallelismController : public QObject
{
    Q_OBJECT
public:

    static GWSParallelismController* globalInstance();

    // GETTERS
    int getThreadsCount() const;

    // METHODS
    QThread* getThread();
    QThread* liberateThread( QThread* thread );

signals:
    void parallelAgentsFinishedSignal();

private:

    GWSParallelismController();
    GWSParallelismController(GWSParallelismController const&);
    ~GWSParallelismController();

    QThread* main_thread;

    QList<QThread*> available_threads; // One per cpu

};

#endif // GWSPARALLELISMCONTROLLER_H
