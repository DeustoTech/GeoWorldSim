#include "Environment.h"

#include <QJsonObject>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

QString GWSEnvironment::SKIP_INDEXING = "skip_indexing";

GWSEnvironment::GWSEnvironment() : GWSEntity(){
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

void GWSEnvironment::registerEntity( QSharedPointer<GWSEntity> entity){
    if( entity->environments_registerd_in.contains( this ) ){
        return;
    }
    entity->environments_registerd_in.append( this );
}

void GWSEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity){
    entity->environments_registerd_in.removeAll( this );
}
