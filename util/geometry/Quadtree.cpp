#include "Quadtree.h"

#include <spatialindex/Region.h>

GWSQuadtree::GWSQuadtree() : QObject(){
    for( int l = this->layer_amount ; l > 0 ; l-- ){
        this->geom_index_layers.insert( l , new QMap< int , QMap< int, QStringList* >* >() );
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
    this->mutex.lock();
    QSharedPointer<GWSGeometry> geom = this->id_to_geometries.value( object_id , Q_NULLPTR );
    this->mutex.unlock();
    return geom;
}

QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
    QList< QSharedPointer<GWSObject> > objects;

    // See the decimal value where min and max diverge
    QString x_decimals = QString::number( qAbs( minX - maxX ) );

    // Count how many 0 it has from the beginning until reaching first number
    int x_decimal_count = 0;
    for( int c = 0 ; c < x_decimals.length() ; c++ ){
        if( x_decimals[c] == '0' ){
            x_decimal_count++;
        } else if( x_decimals[c] == '.' ){
            x_decimal_count = 0;
        } else {
            break;
        }
    }

    // See the decimal value where min and max diverge
    QString y_decimals = QString::number( qAbs( minY - maxY ) );

    // Count how many 0 it has from the beginning until reaching first number
    int y_decimal_count = 0;
    for( int c = 0 ; c < y_decimals.length() ; c++ ){
        if( y_decimals[c] == '0' ){
            y_decimal_count++;
        } else if( y_decimals[c] == '.' ){
            y_decimal_count = 0;
        } else {
            break;
        }
    }

    int x_hash = this->createHash( ( minX + maxX ) / 2 , qMin( x_decimal_count , y_decimal_count ) );
    int y_hash = this->createHash( ( minY + maxY ) / 2  , qMin( x_decimal_count , y_decimal_count ) );

    if( !this->geom_index_layers.keys().contains( qMin( x_decimal_count , y_decimal_count ) ) ){
        return objects;
    }

    if( !this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->keys().contains( x_hash ) ){
        return objects;
    }

    if( !this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->keys().contains( y_hash ) ){
        return objects;
    }

    QStringList* ids = this->geom_index_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->value( y_hash );
    foreach(QString id , *ids ) {
        objects.append( this->id_to_objects.value( id ) );
    }

    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor) {

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

        this->mutex.lock();
        int yhash = this->createHash( coor.getY() , l );
        this->mutex.unlock();

        QStringList* y_layer = x_layer->value( yhash );
        if( !y_layer || y_layer->isEmpty() ){
            continue;
        }

        GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
        QString nearest_object_id;
        foreach (QString object_id, *y_layer) {

            QSharedPointer<GWSGeometry> geom = this->id_to_geometries.value( object_id );
            if( geom ){
                GWSLengthUnit l = geom->getCentroid().getDistance( coor );
                if( l < shortest_distance ){
                    shortest_distance = l;
                    nearest_object_id = object_id;
                }
            }
        }

        if( nearest_object_id.isEmpty() ){
            continue;
        }

        return this->id_to_objects.value( nearest_object_id );
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

        for( int l = this->layer_amount ; l > 0 ; l-- ){

            // Get geohash
            this->mutex.lock();
            int xhash = this->createHash( coor.getX() , l);
            int yhash = this->createHash( coor.getY() , l);
            this->mutex.unlock();

            this->mutex.lock();
            if ( !this->geom_index_layers.value( l )->keys().contains( xhash ) ){
                 this->geom_index_layers.value( l )->insert( xhash , new QMap< int , QStringList* >() );
            }
            this->mutex.unlock();

            this->mutex.lock();
            if ( !this->geom_index_layers.value( l )->value( xhash )->keys().contains( yhash ) ){
                this->geom_index_layers.value( l )->value( xhash )->insert( yhash , new QStringList() );
            }
            this->mutex.unlock();

            // If already here, remove old version
            this->mutex.lock();
            if( this->id_to_geometries.keys().contains( object_id ) ){
                QSharedPointer< GWSGeometry > previous_geom = this->id_to_geometries.value( object_id );
                GWSCoordinate previous_coor = previous_geom->getCentroid();
                int previous_xhash = this->createHash(previous_coor.getX() , l );
                int previous_yhash = this->createHash(previous_coor.getY() , l );
                this->geom_index_layers.value( l )->value( previous_xhash )->value( previous_yhash )->removeAll( object_id );
            }
            this->mutex.unlock();

            // Insert new version
            this->mutex.lock();
            this->geom_index_layers.value( l )->value( xhash )->value( yhash )->append( object_id );
            this->mutex.unlock();
        }

        this->mutex.lock();
        this->id_to_objects.insert( object_id , object );
        this->id_to_geometries.insert( object_id , geom );
        this->mutex.unlock();
   }

}

void GWSQuadtree::remove(QSharedPointer<GWSObject> object){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();
    QSharedPointer<GWSGeometry> object_geom = this->id_to_geometries.value( object_id );

    for( int l = this->layer_amount ; l > 0 ; l-- ){

        int xhash = this->createHash( object_geom->getCentroid().getX() , l );
        int yhash = this->createHash( object_geom->getCentroid().getY() , l );

        this->geom_index_layers.value( l )->value( xhash )->value( yhash )->removeAll( object_id );

    }
    this->id_to_objects.remove( object_id );
    this->id_to_geometries.remove( object_id );
}


int GWSQuadtree::createHash(double value, int decimal_amount){
    try {
        int multiplier = qPow( 10 , decimal_amount );
        return qFloor( value * multiplier );
    } catch (std::exception e ){
    }
    return 1;
}
