#include "GridEnvironment.h"

#include <QtConcurrent/QtConcurrent>

#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryTransformators.h"

QString GWSGridEnvironment::GRID_PROP = "grid";

GWSGridEnvironment* GWSGridEnvironment::globalInstance(){
    static GWSGridEnvironment instance;
    return &instance;
}

GWSGridEnvironment::GWSGridEnvironment() : GWSEnvironment() {
    qInfo() << "Grid environment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSGridEnvironment::~GWSGridEnvironment(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

const GWSGeometry GWSGridEnvironment::getBounds() const{
    GWSGeometry bounds;
    foreach( QSharedPointer<GWSGrid> t , this->environment_entity_grids.values() ) {
        bounds = GWSGeometryTransformators::transformToFit( bounds , t->getBounds() );
    }
    return bounds;
}

const GWSGeometry GWSGridEnvironment::getBounds( QString class_name ) const {
    if( this->environment_entity_grids.keys().contains( class_name ) ){
        return this->environment_entity_grids.value( class_name )->getBounds();
    }
    return GWSGeometry();
}

const QJsonValue GWSGridEnvironment::getValue( QString class_name , GWSGeometry geom ) const {
    if( this->environment_entity_grids.keys().contains( class_name ) ){
        return this->environment_entity_grids.value( class_name )->getValue( geom );
    }
    return QJsonValue::Null;
}

const QJsonValue GWSGridEnvironment::getValue( QSharedPointer<GWSEntity> entity ) const {
    if( this->environment_entity_grids.keys().contains( entity->metaObject()->className() ) ){
        GWSGeometry geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        return this->environment_entity_grids.value( entity->metaObject()->className() )->getValue( geom );
    }
    return QJsonValue::Null;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerEntity( QSharedPointer<GWSEntity> entity ){

    if( entity.isNull() || entity->getProperty( GRID_PROP ).isNull() ){
        return;
    }

    GWSEnvironment::registerEntity( entity );

    QJsonObject json = entity->getProperty( GRID_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    /*GWSGrid grid = GWSGrid( json );
    if( !grid.isValid() ){
        return;
    }

    this->upsertEntityToIndex( agent , edge );

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

    // Listen to agent property changes
    agent->subscribe( EDGE_PROP , [this](QSharedPointer<GWSObject> entity , QString property_name ){
        this->upsertEntityToIndex( entity.dynamicCast<GWSEntity>() , GWSGeometry( entity->getProperty( property_name ).toObject() ) );
    });*/

}

void GWSGridEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity ){
    GWSEnvironment::unregisterEntity( entity );
    //this->disconnect( entity.data() , &GWSEntity::entityPropertyChangedSignal , this , &GWSGridEnvironment::entityPropertyChanged );
}


/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSGridEnvironment::upsertValueToGrid( QSharedPointer<GWSEntity> entity , const QJsonValue& value ){

    GWSGeometry entity_geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    if( !entity_geom.isValid() ){ return; }

    GWSGeometry bounds = GWSPhysicalEnvironment::globalInstance()->getBounds();

    foreach (QJsonValue v , entity->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lockForRead();
        if( !this->environment_entity_grid_types.contains( family ) ){
            this->mutex.unlock();

            this->mutex.lockForWrite();
            this->environment_entity_grid_types.append( family );
            this->environment_entity_grids.insert( family , QSharedPointer<GWSGrid>( new GWSGrid( bounds , 1000 , 1000 , "total" ) ) );
        }
        this->mutex.unlock();

        QtConcurrent::run([this , entity_geom , value , family] {
            this->environment_entity_grids.value( family )->addValue( entity_geom , value );
        });
    }
}

void GWSGridEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == GRID_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        GWSEntity* entity = dynamic_cast<GWSEntity*>( object );
        if( !entity ){ return; }
        this->upsertValueToGrid( entity->getSharedPointer() , entity->getProperty( GRID_PROP ) );
    }
}
