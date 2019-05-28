#include "Environment.h"

#include <QJsonObject>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

QString GWSEnvironment::SKIP_INDEXING = "skip_indexing";

GWSEnvironment::GWSEnvironment() : GWSAgent(){
    emit this->environmentCreatedSignal();
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
