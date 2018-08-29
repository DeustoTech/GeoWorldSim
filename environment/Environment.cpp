#include "Environment.h"

#include <QJsonObject>
#include <QPainter>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

GWSEnvironment* GWSEnvironment::globalInstance(){
    static GWSEnvironment instance;
    return &instance;
}

GWSEnvironment::GWSEnvironment() : GWSAgent(){
    emit this->environmentCreatedSignal();
}

GWSEnvironment::~GWSEnvironment(){
    emit this->environmentDeletedSignal();
}

/**********************************************************************
 AGENTS METHODS
**********************************************************************/

void GWSEnvironment::registerAgent( QSharedPointer<GWSAgent> agent){
    if( agent->environments_registerd_in.contains( this ) ){
        return;
    }

    agent->environments_registerd_in.append( this );
}

void GWSEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent){
    agent->environments_registerd_in.removeAll( this );
    foreach( GWSEnvironment* e , this->sub_environments ){
        e->unregisterAgent( agent );
    }
}

void GWSEnvironment::registerSubenvironment(GWSEnvironment *subenvironment){
    if( subenvironment != this ){
        this->sub_environments.append( subenvironment );
    }
}
