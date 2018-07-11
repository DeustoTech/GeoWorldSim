#include "Environment.h"

#include <QJsonObject>
#include <QPainter>
#include <QTimer>

#include "util/parallelism/ParallelismController.h"

GWSEnvironment* GWSEnvironment::globalInstance(){
    static GWSEnvironment instance;
    return &instance;
}

GWSEnvironment::GWSEnvironment() : GWSAgent( Q_NULLPTR ){
    this->moveToThread( GWSParallelismController::globalInstance()->getThread( qrand() ) );
    emit this->environmentCreatedSignal();
}

GWSEnvironment::~GWSEnvironment(){
    emit this->environmentDeletedSignal();
}


/**********************************************************************
 EXPORTERS
**********************************************************************/

/**
 * @brief Agent::toImage
 * @return
 */
QImage GWSEnvironment::toImage( unsigned int image_width, unsigned int image_height, QStringList class_names){

    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    return image;
}

/**********************************************************************
 AGENTS METHODS
**********************************************************************/

void GWSEnvironment::registerAgent(GWSAgent *agent){
    foreach( GWSEnvironment* e , this->sub_environments ){
        e->registerAgent( agent );
    }
}

void GWSEnvironment::unregisterAgent(GWSAgent *agent){
    foreach( GWSEnvironment* e , this->sub_environments ){
        e->unregisterAgent( agent );
    }
}

void GWSEnvironment::registerSubenvironment(GWSEnvironment *subenvironment){
    if( subenvironment != this ){
        this->sub_environments.append( subenvironment );
    }
}
