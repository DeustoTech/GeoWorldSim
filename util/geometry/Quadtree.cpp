#include "Quadtree.h"

#include <spatialindex/Region.h>

GWSQuadtree::GWSQuadtree() : QObject(){
    std::string filename = QString("%1-%2-index").arg( qrand() ).arg( qrand() ).toStdString();
    SpatialIndex::IStorageManager* storage = StorageManager::createNewDiskStorageManager( filename , 4096 );
    //SpatialIndex::IStorageManager* storage = StorageManager::createNewMemoryStorageManager();
    SpatialIndex::id_type index_identifier;
    this->inner_index = SpatialIndex::RTree::createNewRTree( *storage , 0.7 , 1000 , 1000 , 2 , SpatialIndex::RTree::RV_RSTAR, index_identifier );
    if( !this->inner_index->isIndexValid() ){
        qWarning() << "ERROR CREATING INDEX";
    }
}

GWSQuadtree::~GWSQuadtree(){
    IStatistics* stat;
    this->inner_index->getStatistics( &stat );
    foreach (QString object_id , this->id_to_objects.keys() ) {
        SpatialIndex::Region* region = this->inner_index_geometries.value( object_id );
        this->inner_index->deleteData( *region , this->inner_index_ids.value( object_id ) );
        delete region;
    }

}

/*QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements( GWSCoordinate coor ) {
    return this->getElements<GWSObject>( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
}

template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements( GWSCoordinate coor ) {

}

template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements( QSharedPointer<GWSGeometry> geom ) {
    QList< QSharedPointer<T> > intersecting_agents;
    foreach( QSharedPointer<GWSObject> o , this->getElements<GWSObject>( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() ) ){
        QSharedPointer<GWSGeometry> o_geom = this->id_to_geometries.value( o->getId() );
        if( geom->intersects( o_geom ) ){
            intersecting_agents.append( o.dynamicCast<T>() );
        }
    }
    return intersecting_agents;
}


template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {

}*/

QSharedPointer<GWSGeometry> GWSQuadtree::getGeometry( QString object_id ){
    return this->id_to_geometries.value( object_id , Q_NULLPTR );
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
    QList< QSharedPointer<GWSObject> > objects;

    double min_point[2] = {minX , minY};
    double max_point[2] = {maxX , maxY};
    SpatialIndex::Region *region = new SpatialIndex::Region( min_point , max_point , 2 );

    GWSQuadtreeVisitor visitor;
    this->inner_index->intersectsWithQuery( *region , visitor );
    foreach (SpatialIndex::id_type v , visitor.visited ) {
        objects.append( this->id_to_objects.value( this->inner_index_ids.key( v ) ) );
    }
    delete region;
    return objects;
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getNearestElements(GWSCoordinate coor, unsigned int amount) {
    QList< QSharedPointer<GWSObject> > objects;

    double p[2] = {coor.getX() , coor.getY()};
    SpatialIndex::Point point = SpatialIndex::Point( p , 2 );

    if( !this->inner_index->isIndexValid() ){
        qWarning() << "ERROR CREATING INDEX";
    }

    GWSQuadtreeVisitor visitor;

    this->inner_index->nearestNeighborQuery( amount , point , visitor );
    foreach (SpatialIndex::id_type v , visitor.visited ) {
        objects.append( this->id_to_objects.value( this->inner_index_ids.key( v ) ) );
    }

    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor){
    QList< QSharedPointer<GWSObject> > objects = this->getNearestElements( coor , 1 );
    return ( objects.isEmpty() ? Q_NULLPTR : objects.at(0) );
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getNearestElements(QSharedPointer<GWSGeometry> geom , unsigned int amount ) {
    QList< QSharedPointer<GWSObject> > objects;

    double min_point[2] = {geom->getGeometryMinX() , geom->getGeometryMinY()};
    double max_point[2] = {geom->getGeometryMaxX() , geom->getGeometryMaxY()};
    SpatialIndex::Region *region = new SpatialIndex::Region( min_point , max_point , 2 );

    GWSQuadtreeVisitor visitor;
    this->inner_index->nearestNeighborQuery( amount , *region , visitor );
    foreach (SpatialIndex::id_type v , visitor.visited ) {
        objects.append( this->id_to_objects.value( this->inner_index_ids.key( v ) ) );
    }

    delete region;
    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(QSharedPointer<GWSGeometry> geom){
    QList< QSharedPointer<GWSObject> > objects = this->getNearestElements( geom , 1 );
    return ( objects.isEmpty() ? Q_NULLPTR : objects.at(0) );
}

void GWSQuadtree::upsert( QSharedPointer<GWSObject> agent , GWSCoordinate coor ){
    QSharedPointer<GWSGeometry> geom = QSharedPointer<GWSGeometry>( new GWSGeometry() );
    geom->transformMove( coor );
    this->upsert( agent , geom );
    geom.clear();
}

void GWSQuadtree::upsert( QSharedPointer<GWSObject> object , QSharedPointer<GWSGeometry> geom ){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();

    if( geom && geom->isGeometryValid() ){

        // Check if already here
        SpatialIndex::id_type id = this->inner_index_ids.value( object_id , -1 );
        SpatialIndex::Region *region;
        if( id >= 0 ){
            region = this->inner_index_geometries.value( object_id );
            this->inner_index->deleteData( *region , id );
            delete region;
        } else {
            // Create libspatialindex id
            id = ++this->inner_index_last_id;
        }

        // Create libspatialindex geometry
        double min_point[2] = {geom->getGeometryMinX() , geom->getGeometryMinY()};
        double max_point[2] = {geom->getGeometryMaxX() , geom->getGeometryMaxY()};
        region = new SpatialIndex::Region( min_point , max_point , 2 );

        // Insert
        this->inner_index->insertData( 0 , 0 , *region , id );
        this->inner_index_ids.insert( object_id , id );
        this->inner_index_geometries.insert( object_id , region );

        this->id_to_objects.insert( object_id , object );
        this->id_to_geometries.insert( object_id , geom );
    }

}

void GWSQuadtree::remove(QSharedPointer<GWSObject> object){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();

    // Check if exists
    if( this->inner_index_ids.value( object_id , -1 ) < 0 ){
        return;
    }

    // Remove from libspatialindex
    SpatialIndex::Region *region = this->inner_index_geometries.value( object_id );
    SpatialIndex::id_type id = this->inner_index_ids.value( object_id );
    this->inner_index->deleteData( *region , id );
    this->inner_index_ids.remove( object_id );
    this->inner_index_geometries.remove( object_id );
    delete region;

    this->id_to_objects.remove( object_id );
    this->id_to_geometries.remove( object_id );
}

