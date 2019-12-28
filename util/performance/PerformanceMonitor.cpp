#include "PerformanceMonitor.h"

#include <QSysInfo>
#include <unistd.h>
#include <sys/resource.h>

#include "utils/io/log/Logger.h"

GWSPerformanceMonitor::GWSPerformanceMonitor() : QObject( Q_NULLPTR ){

}

/**********************************************************************
 SLOTS
**********************************************************************/

void GWSPerformanceMonitor::startWatchingPerformance(){
    this->max_running_agents_count = 0;
    this->max_threads_count = 0;

    this->elapsed_time.start();

    if( !this->timer ){
        this->timer = new QTimer( this );
    }
    this->timer->setSingleShot( false );
    this->connect( this->timer , SIGNAL(timeout()) , this , SLOT(storeCurrentPerformance()) );
    this->timer->start( 5000 ); // Invoke timeout signal every 5 seconds
}

void GWSPerformanceMonitor::storeCurrentPerformance(){

}

void GWSPerformanceMonitor::stopWatchingPerformance(){
    this->elapsed_time.msecsSinceReference();

    this->timer->stop();
    this->disconnect( this->timer , SIGNAL(timeout()) , this , SLOT(storeCurrentPerformance()) );
    this->timer->deleteLater();
    this->timer = 0;
}


/**********************************************************************
 STATIC
**********************************************************************/

QStringList GWSPerformanceMonitor::getSystemInfo(){
    QSysInfo sys;
    QStringList info;
    info << QString("Product - %1").arg( sys.productType() );
    info << QString("Product Version - %1").arg( sys.productVersion() );
    info << QString("Product String - %1").arg( sys.prettyProductName() );
    info << QString("CPU Architecture - %1").arg( sys.currentCpuArchitecture() );
    info << QString("Kernel - %1").arg( sys.kernelType() );
    info << QString("Kernel Version - %1").arg( sys.kernelVersion() );
    return info;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QStringList GWSPerformanceMonitor::getUsage(){

    QStringList info;
    // http://linux.die.net/man/2/getrusage

    //rusage st;
    //int s = getrusage( RUSAGE_SELF , &st );
    return info;
}
