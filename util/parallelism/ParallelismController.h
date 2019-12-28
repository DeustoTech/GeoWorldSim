#ifndef PARALLELISMCONTROLLER_H
#define PARALLELISMCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QThread>

namespace geoworldsim {
namespace parallel {


class ParallelismController : public QObject
{
    Q_OBJECT
public:

    static ParallelismController* globalInstance();

    // GETTERS
    int getThreadsCount() const;

    // METHODS
    QThread* getThread();
    QThread* getMainThread();
    QThread* liberateThread( QThread* thread );

signals:
    void parallelAgentsFinishedSignal();

private:

    ParallelismController();
    ParallelismController(ParallelismController const&);
    ~ParallelismController();

    QThread* main_thread;

    QList<QThread*> available_threads; // One per cpu

};


}
}


#endif // PARALLELISMCONTROLLER_H
