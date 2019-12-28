#include "VideoExporter.h"

#include <QImage>
#include <QPainter>

#include "utils/parallelism_controller/ParallelismController.h"

VideoExporter::VideoExporter(GSSEnvironment *environment, GSSEnvelope envelope, QObject *parent) : QObject( 0 ){
    Q_UNUSED( parent )
    this->environment = environment;
    this->envelope = envelope;

    this->moveToThread( ParallelismController::getInstance()->getThread( qrand() ) );
}

/**********************************************************************
 METHODS
**********************************************************************/

void VideoExporter::start(){
    // Set last datetime
    this->last_rendered_datetime = 0;

    // Connect behaved
    QObject::connect( this->environment ,  &GSSEnvironment::agentBehavedSignal , this , &VideoExporter::createFrame );
}

void VideoExporter::stop(){
    // Connect behaved
    QObject::disconnect( this->environment , &GSSEnvironment::agentBehavedSignal , this , &VideoExporter::createFrame );
}

/**********************************************************************
 SLOTS
**********************************************************************/

void VideoExporter::createFrame(QString class_name, QString id){
    Q_UNUSED( class_name )
    Q_UNUSED( id )

    // Check if time environment has gone back
    quint64 now = this->environment->getTimeEnvironment()->getCurrentDateTime();
    if( this->last_rendered_datetime >= now ){
        return;
    }

    this->last_rendered_datetime = now;

    QImage image = QImage( 2048 , 2048 , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );

    foreach(QString s , this->export_agent_types){
        foreach(Agent* a , this->environment->getByClass( s ) ){
            painter.drawImage( 0 , 0 , a->toImage( this->envelope , image.width() , image.height() ) );
        }
    }

    image.save( QString("%1.jpg").arg( this->last_rendered_datetime ) );
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QVideoFrame VideoExporter::generateFrame(int step){
    QVideoFrame frame;
    return frame;
}
