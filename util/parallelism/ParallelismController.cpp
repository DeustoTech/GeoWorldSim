#include "ParallelismController.h"

#include <QThreadPool>
#include <QtMath>
#include <QDebug>

#include "../../app/App.h"

geoworldsim::parallel::ParallelismController* geoworldsim::parallel::ParallelismController::globalInstance(){
    static geoworldsim::parallel::ParallelismController instance;
    return &instance;
}

geoworldsim::parallel::ParallelismController::ParallelismController() : QObject(){
    this->main_thread = this->thread();

    // EDIT : /etc/security/limits.conf
    // echo 1000000 > /proc/sys/fs/file-max
    // echo 100000 > /proc/sys/kernel/threads-max
    // lsof | grep QtCreator

    // Remove one to be left for main thread
    int thread_limit = App::globalInstance()->getConfiguration().value( "max_threads" ).toInt( QThreadPool::globalInstance()->maxThreadCount() );
    thread_limit = qMax( 1 , thread_limit );

    for(int i = 0; i < thread_limit; i++){

        // Create thread
        QThread* thread = new QThread();
        thread->setObjectName( QString("GWS allocated %1").arg( this->available_threads.size() ) );
        this->available_threads.append( thread );
        thread->start();
    }

    qInfo() << QString("Parallelism Controller created with %1 threads").arg( this->available_threads.size() );
}

geoworldsim::parallel::ParallelismController::~ParallelismController(){
    foreach (QThread* t , this->available_threads) {
        t->exit();
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

int geoworldsim::parallel::ParallelismController::getThreadsCount() const{
    return this->available_threads.size();
}

/**********************************************************************
 METHODS
**********************************************************************/

QThread* geoworldsim::parallel::ParallelismController::getThread(){
    return this->available_threads.at( qrand() % this->available_threads.size() );
}

QThread* geoworldsim::parallel::ParallelismController::getMainThread(){
    return this->main_thread;
}

QThread* geoworldsim::parallel::ParallelismController::liberateThread(QThread *thread){
    Q_UNUSED(thread)
    return this->main_thread;
}
