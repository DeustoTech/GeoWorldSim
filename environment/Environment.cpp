#include "Environment.h"

#include <QJsonObject>
#include <QPainter>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

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
