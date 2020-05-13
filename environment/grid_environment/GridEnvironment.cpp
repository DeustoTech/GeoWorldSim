#include "GridEnvironment.h"

#include <QtConcurrent/QtConcurrent>

#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryTransformators.h"


QString geoworldsim::environment::GridEnvironment::GRID_PROP = "grid";

geoworldsim::environment::GridEnvironment* geoworldsim::environment::GridEnvironment::globalInstance(){
    static geoworldsim::environment::GridEnvironment instance;
    return &instance;
}

geoworldsim::environment::GridEnvironment::GridEnvironment() : Environment() {
    qInfo() << "Grid environment created";
    environment::EnvironmentsGroup::globalInstance()->addEnvironment( this );
}

geoworldsim::environment::GridEnvironment::~GridEnvironment(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

const geoworldsim::geometry::Geometry geoworldsim::environment::GridEnvironment::getBounds() const{
    geometry::Geometry bounds;
    foreach( QSharedPointer<grid::Grid> t , this->environment_entity_grids.values() ) {
        bounds = geometry::GeometryTransformators::transformToFit( bounds , t->getBounds() );
    }
    return bounds;
}

const geoworldsim::geometry::Geometry geoworldsim::environment::GridEnvironment::getBounds( QString class_name ) const {
    QSharedPointer<grid::Grid> grid = this->environment_entity_grids.value( class_name.toStdString() , Q_NULLPTR );
    if( grid ){
        return grid->getBounds();
    }
    return geometry::Geometry();
}

const QJsonValue geoworldsim::environment::GridEnvironment::getValue( QString class_name , geometry::Geometry geom ) const {
    QSharedPointer<grid::Grid> grid = this->environment_entity_grids.value( class_name.toStdString() , Q_NULLPTR );
    if( grid ){
        return grid->getValue( geom );
    }
    return QJsonValue::Null;
}

const QJsonValue geoworldsim::environment::GridEnvironment::getValue( QSharedPointer<Entity> entity ) const {
    QSharedPointer<grid::Grid> grid = this->environment_entity_grids.value( entity->metaObject()->className() , Q_NULLPTR );
    if( grid ){
        geometry::Geometry geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        return grid->getValue( geom );
    }
    return QJsonValue::Null;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void geoworldsim::environment::GridEnvironment::registerEntity( QSharedPointer<Entity> entity ){

    if( entity.isNull() || entity->getProperty( GRID_PROP ).isNull() ){
        return;
    }

    environment::Environment::registerEntity( entity );

    QJsonObject json = entity->getProperty( GRID_PROP ).toObject();

    if( json.value( SKIP_INDEXING ).toBool() ){
        return;
    }

    if( json.value( SKIP_SUBSCRIBING ).toBool() ){
        return;
    }

}

void geoworldsim::environment::GridEnvironment::unregisterEntity( QSharedPointer<Entity> entity ){
    environment::Environment::unregisterEntity( entity );
    //this->disconnect( entity.data() , &GWSEntity::entityPropertyChangedSignal , this , &geoworldsim::environment::GridEnvironment::entityPropertyChanged );
}


/**********************************************************************
 PROTECTED
**********************************************************************/

void geoworldsim::environment::GridEnvironment::upsertValueToGrid( QSharedPointer<Entity> entity , const QJsonValue& value ){

    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    if( !entity_geom.isValid() ){ return; }

    geometry::Geometry bounds = environment::PhysicalEnvironment::globalInstance()->getBounds();

    foreach (QJsonValue v , entity->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lockForRead();
        QSharedPointer<grid::Grid> grid = this->environment_entity_grids.value( family.toStdString() , Q_NULLPTR );

        if( !grid ){
            this->mutex.unlock();

            this->mutex.lockForWrite();
            grid = QSharedPointer<grid::Grid>( new grid::Grid( bounds , 1000 , 1000 , "total" ) );
            this->environment_entity_grids.insert( family.toStdString() , grid );
        }
        this->mutex.unlock();

        QtConcurrent::run([this , entity_geom , value , grid] {
            grid->addValue( entity_geom , value );
        });
    }
}

void geoworldsim::environment::GridEnvironment::entityPropertyChanged( QString property_name ){
    if( property_name == GRID_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        Entity* entity = dynamic_cast<Entity*>( object );
        if( !entity ){ return; }
        this->upsertValueToGrid( entity->getSharedPointer() , entity->getProperty( GRID_PROP ) );
    }
}

