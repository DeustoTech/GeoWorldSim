#include "Quadtree.h"

#include <spatialindex/Region.h>
#include "GeometryGetters.h"
#include "GeometryTransformators.h"

GWSQuadtree::GWSQuadtree() : QObject(){
    for( int l = this->layer_amount ; l > 0 ; l-- ){
        this->geom_index_layers.insert( l , new QMap< int , QMap< int, QStringList* >* >() );
    }
}

GWSQuadtree::~GWSQuadtree(){
    this->id_to_geometries.clear();
}

QStringList GWSQuadtree::getElements(){
    return this->ids_contained;
}

const GWSNewGeometry GWSQuadtree::getGeometry( QString object_id ){
    this->mutex.lockForRead();
    const GWSNewGeometry geom = this->id_to_geometries.value( object_id );
    this->mutex.unlock();
    return geom;
}

QStringList GWSQuadtree::getElements( GWSCoordinate coor ){
    return this->getElements( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
}

QStringList GWSQuadtree::getElements(const GWSNewGeometry geom){
    return this->getElements(
                GWSGeometryGetters::getGeometryMinX( geom ) ,
                GWSGeometryGetters::getGeometryMaxX( geom ) ,
                GWSGeometryGetters::getGeometryMinY( geom ) ,
                GWSGeometryGetters::getGeometryMaxY( geom ) );
}

QStringList GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
    QStringList objects_ids;

    // See the decimal value where min and max diverge
    QString x_max = QString::number( maxX );
    QString x_min = QString::number( minX );

    // Count how many 0 it has from the beginning until reaching first number
    int x_decimal_count = 0;
    for( int c = 0 ; c < x_max.length() ; c++ ){
        if( x_max[c] == '.' || x_min[c] == '.' ){
            x_decimal_count = 0;
            continue;
        }

        if( x_max[c] == x_min[c] ){
            x_decimal_count++;
        } else {
            break;
        }
    }

    // See the decimal value where min and max diverge
    QString y_max = QString::number( maxY );
    QString y_min = QString::number( minY );

    // Count how many 0 it has from the beginning until reaching first number
    int y_decimal_count = 0;
    for( int c = 0 ; c < y_max.length() ; c++ ){
        if( y_max[c] == '.' || y_min[c] == '.' ){
            y_decimal_count = 0;
            continue;
        }

        if( y_max[c] == y_min[c] ){
            y_decimal_count++;
        } else {
            break;
        }
    }

    int x_hash = this->createHash( ( minX + maxX ) / 2.0 , qMin( x_decimal_count , y_decimal_count ) );
    int y_hash = this->createHash( ( minY + maxY ) / 2.0  , qMin( x_decimal_count , y_decimal_count ) );

    if( !this->geom_index_layers.keys().contains( qMin( x_decimal_count , y_decimal_count ) ) ){
        return objects_ids;
    }

    if( !this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->keys().contains( x_hash ) ){
        return objects_ids;
    }

    if( !this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->keys().contains( y_hash ) ){
        return objects_ids;
    }

    this->mutex.lockForWrite();
    QStringList* ids = this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->value( y_hash );
    foreach(QString id , *ids ) {
        objects_ids.append( id );
    }
    this->mutex.unlock();

    return objects_ids;
}

QString GWSQuadtree::getNearestElement(GWSCoordinate coor) {

    for( int l = this->layer_amount ; l > 0 ; l-- ){

        QMap< int , QMap< int , QStringList* >* >* layer = this->geom_index_layers.value( l );
        if( !layer ){
            continue;
        }

        int xhash = this->createHash( coor.getX() , l );

        QMap< int, QStringList* >* x_layer = layer->value( xhash );
        if( !x_layer ){
            continue;
        }

        int yhash = this->createHash( coor.getY() , l );

        QStringList* y_layer = x_layer->value( yhash );
        if( !y_layer || y_layer->isEmpty() ){
            continue;
        }

        GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
        QString nearest_object_id;
        foreach (QString object_id, *y_layer) {

            const GWSNewGeometry geom = this->id_to_geometries.value( object_id );
            if( geom.isValid() ){
                GWSLengthUnit l = geom.getCentroid().getDistance( coor );
                if( l < shortest_distance ){
                    shortest_distance = l;
                    nearest_object_id = object_id;
                }
            }
        }

        if( nearest_object_id.isEmpty() ){
            continue;
        }

        return nearest_object_id;
    }
    return QString();
}

QString GWSQuadtree::getNearestElement( const GWSNewGeometry geom ) {
    return this->getNearestElement( geom.getCentroid() );
}

void GWSQuadtree::upsert( QString object_id , GWSCoordinate coor ){
    GWSNewGeometry geom = GWSGeometryTransformators::transformMove( GWSNewGeometry() , coor );
    this->upsert( object_id , geom );
}

void GWSQuadtree::upsert( QString object_id , const GWSNewGeometry geom ){

    if( object_id.isEmpty() ){
        return;
    }

    GWSCoordinate coor = geom.getCentroid();

    if ( coor.isValid() ){

        for( int l = this->layer_amount ; l > 0 ; l-- ){

            //QtConcurrent::run([this , coor , l , object_id , object] {

                // Get geohash
                int xhash = this->createHash( coor.getX() , l );
                int yhash = this->createHash( coor.getY() , l );

                this->mutex.lockForRead();
                if ( !this->geom_index_layers.value( l )->keys().contains( xhash ) ){
                     this->geom_index_layers.value( l )->insert( xhash , new QMap< int , QStringList* >() );
                }
                this->mutex.unlock();

                this->mutex.lockForRead();
                if ( !this->geom_index_layers.value( l )->value( xhash )->keys().contains( yhash ) ){
                    this->geom_index_layers.value( l )->value( xhash )->insert( yhash , new QStringList() );
                }
                this->mutex.unlock();

                // If already here, remove old version
                if( this->ids_contained.contains( object_id ) ){
                    this->mutex.lockForRead();
                    GWSNewGeometry previous_geom = this->id_to_geometries.value( object_id );
                    this->mutex.unlock();
                    GWSCoordinate previous_coor = previous_geom.getCentroid();
                    int previous_xhash = this->createHash( previous_coor.getX() , l );
                    int previous_yhash = this->createHash( previous_coor.getY() , l );
                    this->mutex.lockForRead();
                    this->geom_index_layers.value( l )->value( previous_xhash )->value( previous_yhash )->removeAll( object_id );
                    this->mutex.unlock();
                }

                // Insert new version
                this->mutex.lockForWrite();
                this->geom_index_layers.value( l )->value( xhash )->value( yhash )->append( object_id );
                this->mutex.unlock();
            //});
        }

        this->mutex.lockForWrite();
        this->id_to_geometries.insert( object_id , geom );
        this->ids_contained.append( object_id );
        this->mutex.unlock();
   }

}

void GWSQuadtree::remove(QString object_id){

    if( object_id.isEmpty() ){
        return;
    }

    const GWSNewGeometry object_geom = this->id_to_geometries.value( object_id );

    if( !object_geom.isValid() ){
        return;
    }

    for( int l = this->layer_amount ; l > 0 ; l-- ){

        QtConcurrent::run([ this , l , object_geom , object_id ] {

            int xhash = this->createHash( object_geom.getCentroid().getX() , l );
            int yhash = this->createHash( object_geom.getCentroid().getY() , l );
            this->geom_index_layers.value( l )->value( xhash )->value( yhash )->removeAll( object_id );

        });
    }

    this->mutex.lockForWrite();
    this->id_to_geometries.remove( object_id );
    this->ids_contained.removeAll( object_id );
    this->mutex.unlock();

}


int GWSQuadtree::createHash(double value, int decimal_amount) const{
    try {
        int multiplier = qPow( 10 , decimal_amount );
        return qFloor( value * multiplier );
    } catch (std::exception e ){
    }
    return 1;
}
