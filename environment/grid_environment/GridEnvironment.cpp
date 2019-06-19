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
    return QJsonValue::Undefined;
}

const QJsonValue GWSGridEnvironment::getValue( QSharedPointer<GWSEntity> entity ) const {
    if( this->environment_entity_grids.keys().contains( entity->metaObject()->className() ) ){
        GWSGeometry geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        return this->environment_entity_grids.value( entity->metaObject()->className() )->getValue( geom );
    }
    return QJsonValue::Undefined;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerEntity( QSharedPointer<GWSEntity> entity ){

    if( entity.isNull() || entity->getProperty( GRID_PROP ).isUndefined() ){
        return;
    }

    GWSEnvironment::registerEntity( entity );

    if( entity->getProperty( SKIP_INDEXING ).toBool() ){
        return;
    }

    // GEOMETRY (comes as a QJSONOBJECT, need to extract it and build a GWSGEOMETRY )
    QJsonValue value = entity->getProperty( GRID_PROP );

    // Listen to entity property changes
    this->connect( entity.data() , &GWSEntity::propertyChangedSignal , this , &GWSGridEnvironment::entityPropertyChanged );

    if( value.isUndefined() ){
        return;
    }

    this->upsertValueToGrid( entity , value );
}

void GWSGridEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity ){

}


/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSGridEnvironment::upsertValueToGrid( QSharedPointer<GWSEntity> entity , QJsonValue value ){

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
