﻿#include "Environment.h"

#include <QJsonObject>
#include <QPainter>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

GWSEnvironment::GWSEnvironment() : GWSAgent(){
    emit this->environmentCreatedSignal();
    this->moveToThread( GWSParallelismController::globalInstance()->getThread( qrand() ) );
}

GWSEnvironment::~GWSEnvironment(){
    emit this->environmentDeletedSignal();
}

/**********************************************************************
 AGENTS METHODS
**********************************************************************/

QJsonObject GWSEnvironment::serialize() const{
    return GWSObject::serialize(); // Jump all agent serialisation
}

void GWSEnvironment::registerAgent( QSharedPointer<GWSAgent> agent){
    if( agent->environments_registerd_in.contains( this ) ){
        return;
    }
    agent->environments_registerd_in.append( this );
}

void GWSEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent){
    agent->environments_registerd_in.removeAll( this );
}
