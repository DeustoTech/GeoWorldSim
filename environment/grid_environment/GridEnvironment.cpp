#include "GridEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryTransformators.h"

QString GWSGridEnvironment::GRID_PROP = "noise";

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
    foreach( QSharedPointer<GWSGrid> t , this->environment_agent_grids.values() ) {
        bounds = GWSGeometryTransformators::transformToFit( bounds , t->getBounds() );
    }
    return bounds;
}

const GWSGeometry GWSGridEnvironment::getBounds( QString class_name ) const {
    if( this->environment_agent_grids.keys().contains( class_name ) ){
        return this->environment_agent_grids.value( class_name )->getBounds();
    }
    return GWSGeometry();
}

const QJsonValue GWSGridEnvironment::getValue( QString class_name , GWSGeometry geom ) const {
    if( this->environment_agent_grids.keys().contains( class_name ) ){
        return this->environment_agent_grids.value( class_name )->getValue( geom );
    }
    return QJsonValue();
}

const QJsonValue GWSGridEnvironment::getValue( QSharedPointer<GWSAgent> agent ) const {
    if( this->environment_agent_grids.keys().contains( agent->metaObject()->className() ) ){
        GWSGeometry geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        return this->environment_agent_grids.value( agent->metaObject()->className() )->getValue( geom );
    }
    return QJsonValue();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGridEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){

    if( agent.isNull() || agent->getProperty( GRID_PROP ).isNull() ){
        return;
    }

    // GEOMETRY (comes as a QJSONOBJECT, need to extract it and build a GWSGEOMETRY )
    QJsonValue value = agent->getProperty( GRID_PROP ).toObject();

    // Listen to agent property changes
    this->connect( agent.data() , &GWSAgent::propertyChangedSignal , this , &GWSGridEnvironment::agentPropertyChanged );

    if( value.isNull() ){
        return;
    }

    this->upsertValueToGrid( agent , value );

    GWSEnvironment::registerAgent( agent );
}

void GWSGridEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

}


/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSGridEnvironment::upsertValueToGrid( QSharedPointer<GWSAgent> agent , QJsonValue value ){

    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    if( !agent_geom.isValid() ){ return; }

    GWSGeometry bounds = GWSPhysicalEnvironment::globalInstance()->getBounds();

    foreach (QJsonValue v , agent->getInheritanceFamily() ) {

        QString family = v.toString();
        if( family.isEmpty() ){ continue; }

        this->mutex.lockForRead();
        if( !this->environment_agent_grid_types.contains( family ) ){
            this->mutex.unlock();

            this->mutex.lockForWrite();
            this->environment_agent_grid_types.append( family );
            this->environment_agent_grids.insert( family , QSharedPointer<GWSGrid>( new GWSGrid( bounds , 1000 , 1000 ) ) );
        }
        this->mutex.unlock();

        QtConcurrent::run([this , agent_geom , value , family] {
            this->environment_agent_grids.value( family )->setValue( agent_geom , value );
        });
    }
}

void GWSGridEnvironment::agentPropertyChanged( QString property_name ){
    if( property_name == GRID_PROP ){
        QObject* object = QObject::sender();
        if( !object ){ return; }
        GWSAgent* agent = dynamic_cast<GWSAgent*>( object );
        if( !agent ){ return; }
        this->upsertValueToGrid( agent->getSharedPointer() , agent->getProperty( GRID_PROP ) );
    }
}
