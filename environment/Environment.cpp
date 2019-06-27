#include "Environment.h"

#include <QJsonObject>
#include <QTimer>

#include "../../util/parallelism/ParallelismController.h"

QString GWSEnvironment::SKIP_INDEXING = "skip_indexing";
QString GWSEnvironment::SKIP_SUBSCRIBING = "skip_subscribing";

GWSEnvironment::GWSEnvironment() : GWSEntity(){
}

GWSEnvironment::~GWSEnvironment(){
}

/**********************************************************************
 AGENTS METHODS
**********************************************************************/

void GWSEnvironment::deserialize(const QJsonObject &json, QSharedPointer<GWSObject> parent){
    // Avoid GWSEntity deserialisation
    GWSObject::deserialize( json , parent );
}

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
