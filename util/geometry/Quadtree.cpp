#include "Quadtree.h"

#include <spatialindex/Region.h>

GWSQuadtree::GWSQuadtree( QString hash ) : QObject(){
   // std::string filename = hash.toStdString();
   // SpatialIndex::IStorageManager* storage = StorageManager::createNewDiskStorageManager( filename , 4096 );
    //SpatialIndex::IStorageManager* storage = StorageManager::createNewMemoryStorageManager();
   // SpatialIndex::id_type index_identifier;
   /* this->inner_index = SpatialIndex::RTree::createNewRTree( *storage , 0.7 , 100 , 100 , 2 , SpatialIndex::RTree::RV_RSTAR, index_identifier );
    if( !this->inner_index->isIndexValid() ){
        qWarning() << "ERROR CREATING INDEX";
    }*/
}

GWSQuadtree::~GWSQuadtree(){
   // this->inner_index_geometries.clear();
    //this->inner_index_ids.clear();
    this->id_to_objects.clear();
    this->id_to_coordinates.clear();
}



QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(){
    return this->id_to_objects.values();
}

QSharedPointer<GWSGeometry> GWSQuadtree::getGeometry( QString object_id ){
    return this->id_to_coordinates.value( object_id , Q_NULLPTR );
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
   QList< QSharedPointer<GWSObject> > objects;

    double min_point[2] = {minX , minY};
    double max_point[2] = {maxX , maxY};
    SpatialIndex::Region region = SpatialIndex::Region( min_point , max_point , 2 );

    GWSQuadtreeVisitor visitor;
    this->inner_index->intersectsWithQuery( region , visitor );
    foreach (SpatialIndex::id_type v , visitor.visited ) {
        objects.append( this->id_to_objects.value( this->inner_index_ids.key( v ) ) );
    }
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

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor , unsigned int amount ){
    QList< QSharedPointer<GWSObject> > objects = this->getNearestElements( coor , amount );
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

void GWSQuadtree::upsert( QSharedPointer<GWSObject> agent , QSharedPointer<GWSGeometry> geom ){
    this->upsert( agent , geom->getCentroid() );
}

void GWSQuadtree::upsert( QSharedPointer<GWSObject> object , GWSCoordinate coor ){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();

    if ( coor.isValid() ){

        // Check if already here
        QString xstring = QString::number(coor.getX(), 'f', 2);
        QString ystring = QString::number(coor.getY(), 'f', 2);

        if ( !this->geom_index.keys().contains( xstring ) ){
             this->geom_index.insert( xstring , QMap<  QString , QStringList > );
        }

        if ( !this->geom_index.value( xstring ).keys( ystring ) ){
            this->geom_index.value( xstring ).insert( ystring , QStringList() );
        }

        this->geom_index.value( xstring ).value( ystring ).append( object_id );
        this->id_to_objects.insert( object_id , object );
        this->id_to_coordinates.insert( object_id , coor );
   }

}

void GWSQuadtree::remove(QSharedPointer<GWSObject> object){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();
    GWSCoordinate object_coor = this->id_to_coordinates.value( object_id );

    QString xstring = object_coor.getX();
    QString ystring = object_coor.getY();

    this->geom_index.value( xstring ).value( ystring ).removeAll( object_id );

    this->id_to_objects.remove( object_id );
    this->id_to_coordinates.remove( object_coor );
}

