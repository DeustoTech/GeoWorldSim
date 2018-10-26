#include "Quadtree.h"

#include <spatialindex/Region.h>

GWSQuadtree::GWSQuadtree() : QObject(){
    for( int l = this->layer_amount ; l >= 0 ; l-- ){
        this->geom_index_layers.insert( l , new QMap< QString , QMap< QString, QStringList* >* >() );
    }
}

GWSQuadtree::~GWSQuadtree(){
    this->id_to_objects.clear();
    this->id_to_geometries.clear();
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(){
    return this->id_to_objects.values();
}

QSharedPointer<GWSGeometry> GWSQuadtree::getGeometry( QString object_id ){
    return this->id_to_geometries.value( object_id , Q_NULLPTR );
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
    QList< QSharedPointer<GWSObject> > objects;

    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor) {

    for( int l = this->layer_amount ; l >= 0 ; l-- ){

        QString xstring = this->createHash( coor.getX() , l );
        QString ystring = this->createHash( coor.getY() , l );

        QStringList* object_ids = this->geom_index_layers.value( l )->value( xstring )->value( ystring );

        if( object_ids->isEmpty() ){
            continue;
        }

        QString object_id = object_ids->at( qrand() % object_ids->size() );
        return this->id_to_objects.value( object_id );
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement( QSharedPointer<GWSGeometry> geom ) {
    return this->getNearestElement( geom->getCentroid() );
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
    GWSCoordinate coor = geom->getCentroid();

    if ( coor.isValid() ){

        for( int l = this->layer_amount ; l >= 0 ; l-- ){

            // Get geohash
            QString xstring = this->createHash(coor.getX() , l);
            QString ystring = this->createHash(coor.getY() , l);

            this->mutex.lock();
            if ( !this->geom_index_layers.value( l )->keys().contains( xstring ) ){
                 this->geom_index_layers.value( l )->insert( xstring , new QMap< QString , QStringList* >() );
            }
            this->mutex.unlock();

            this->mutex.lock();
            if ( !this->geom_index_layers.value( l )->value( xstring )->keys().contains( ystring ) ){
                this->geom_index_layers.value( l )->value( xstring )->insert( ystring , new QStringList() );
            }
            this->mutex.unlock();

            // If already here, remove old version
            this->mutex.lock();
            if( this->id_to_geometries.keys().contains( object_id ) ){
                QSharedPointer< GWSGeometry > previous_geom = this->id_to_geometries.value( object_id );
                GWSCoordinate previous_coor = previous_geom->getCentroid();
                QString previous_xstring = this->createHash(previous_coor.getX() , l );
                QString previous_ystring = this->createHash(previous_coor.getY() , l );
                this->geom_index_layers.value( l )->value( previous_xstring )->value( previous_ystring )->removeAll( object_id );
            }
            this->mutex.unlock();

            // Insert new version
            this->geom_index_layers.value( l )->value( xstring )->value( ystring )->append( object_id );

        }

        this->id_to_objects.insert( object_id , object );
        this->id_to_geometries.insert( object_id , geom );
   }

}

void GWSQuadtree::remove(QSharedPointer<GWSObject> object){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();
    QSharedPointer<GWSGeometry> object_geom = this->id_to_geometries.value( object_id );

    for( int l = this->layer_amount ; l >= 0 ; l-- ){

        QString xstring = this->createHash( object_geom->getCentroid().getX() , l );
        QString ystring = this->createHash( object_geom->getCentroid().getY() , l );

        this->geom_index_layers.value( l )->value( xstring )->value( ystring )->removeAll( object_id );

    }
    this->id_to_objects.remove( object_id );
    this->id_to_geometries.remove( object_id );
}


QString GWSQuadtree::createHash(double value, int decimal_amount){
    return QString::number( value , 'f' , decimal_amount );
}
