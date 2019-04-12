#include "Quadtree.h"

#include "GeometryGetters.h"
#include "GeometryTransformators.h"

GWSQuadtree::GWSQuadtree() : QObject(){
    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){
        this->quadtree_layers.insert( l , new QMap< int , QMap< int, geos::index::quadtree::Quadtree* >* >() );
    }
}

GWSQuadtree::~GWSQuadtree(){
}

QStringList GWSQuadtree::getElements(){
    return this->ids_contained;
}

const GWSGeometry GWSQuadtree::getGeometry( QString object_id ){
    this->mutex.lockForWrite();
    GWSQuadtreeElement* elm = this->quadtree_elements.value( object_id , Q_NULLPTR );
    this->mutex.unlock();

    if( elm ){ return elm->geometry; }
    return GWSGeometry();
}

QStringList GWSQuadtree::getElements( GWSCoordinate coor ){
    QStringList objects_ids;

    int x_hash = this->createHash( coor.getX() / 2.0 , this->layer_depth_amount );
    int y_hash = this->createHash( coor.getY() / 2.0 , this->layer_depth_amount );

    this->mutex.lockForRead();
    if( !this->quadtree_layers.keys().contains( this->layer_depth_amount ) ){
        this->mutex.unlock();
        return objects_ids;
    }

    if( !this->quadtree_layers.value( this->layer_depth_amount )->keys().contains( x_hash ) ){
        this->mutex.unlock();
        return objects_ids;
    }

    if( !this->quadtree_layers.value( this->layer_depth_amount )->value( x_hash )->keys().contains( y_hash ) ){
        this->mutex.unlock();
        return objects_ids;
    }
    this->mutex.unlock();

    const geos::geom::Envelope env = geos::geom::Envelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    std::vector<void*> ids;

    this->mutex.lockForWrite();
    this->quadtree_layers.value( this->layer_depth_amount )->value( x_hash )->value( y_hash )->query( &env , ids );
    this->mutex.unlock();
    foreach(void* id, ids) {
        GWSQuadtreeElement* ptr = (GWSQuadtreeElement *)( id );
        if( !ptr ){ continue; }
        objects_ids.append( ptr->object_id );
    }

    return objects_ids;
}

QStringList GWSQuadtree::getElements(const GWSGeometry geom){
    return this->getElements(
                GWSGeometryGetters::getGeometryMinX( geom ) ,
                GWSGeometryGetters::getGeometryMaxX( geom ) ,
                GWSGeometryGetters::getGeometryMinY( geom ) ,
                GWSGeometryGetters::getGeometryMaxY( geom ) );
}

QStringList GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) {
    QStringList objects_ids;

    // See the decimal value where min and max diverge
    QString x_max = QString::number( maxX , 'f', this->layer_depth_amount );
    QString x_min = QString::number( minX , 'f', this->layer_depth_amount );

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
    QString y_max = QString::number( maxY , 'f', this->layer_depth_amount );
    QString y_min = QString::number( minY , 'f', this->layer_depth_amount );

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

    this->mutex.lockForRead();
    if( !this->quadtree_layers.keys().contains( qMin( x_decimal_count , y_decimal_count ) ) ){
        this->mutex.unlock();
        return objects_ids;
    }

    if( !this->quadtree_layers.value( qMin( x_decimal_count , y_decimal_count ) )->keys().contains( x_hash ) ){
        this->mutex.unlock();
        return objects_ids;
    }

    if( !this->quadtree_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->keys().contains( y_hash ) ){
        this->mutex.unlock();
        return objects_ids;
    }
    this->mutex.unlock();

    const geos::geom::Envelope env = geos::geom::Envelope( minX , maxX , minY , maxY );
    std::vector<void*> ids;

    this->mutex.lockForWrite();
    this->quadtree_layers.value( qMin( x_decimal_count , y_decimal_count ) )->value( x_hash )->value( y_hash )->query( &env , ids );
    this->mutex.unlock();
    foreach(void* id, ids) {
        GWSQuadtreeElement* ptr = (GWSQuadtreeElement *)( id );
        if( !ptr ){ continue; }
        objects_ids.append( ptr->object_id );
    }

    return objects_ids;
}

QString GWSQuadtree::getNearestElement( GWSCoordinate coor ) {
    GWSGeometry geom = GWSGeometryTransformators::transformMove( GWSGeometry() , coor );
    return this->getNearestElement( geom );
}

QString GWSQuadtree::getNearestElement( const GWSGeometry geom ) {

    if( !geom.isValid() ){ return QString(); }

    GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
    QString nearest_object_id;
    foreach (QString object_id , this->getElements( geom ) ) {

        this->mutex.lockForWrite();
        GWSQuadtreeElement* elm = this->quadtree_elements.value( object_id , Q_NULLPTR );
        this->mutex.unlock();

        if( !elm ){ continue; }
        const GWSGeometry g = elm->geometry;

        if( g.isValid() ){
            GWSLengthUnit l = g.getCentroid().getDistance( geom.getCentroid() );
            if( l < shortest_distance ){
                shortest_distance = l;
                nearest_object_id = object_id;
            }
        }
    }

    return nearest_object_id;
}

void GWSQuadtree::upsert( QString object_id , GWSCoordinate coor ){
    GWSGeometry geom = GWSGeometryTransformators::transformMove( GWSGeometry() , coor );
    this->upsert( object_id , geom );
}

void GWSQuadtree::upsert( QString object_id , const GWSGeometry geom ){

    if( object_id.isEmpty() ){
        return;
    }

    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements.value( object_id , Q_NULLPTR );
    this->mutex.unlock();

    GWSQuadtreeElement* elm = new GWSQuadtreeElement( object_id , geom );

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

        QtConcurrent::run([this , l , elm , previous_elm] {

            if( previous_elm ){

                // Get geohash
                int xhash = this->createHash( previous_elm->geometry.getCentroid().getX() , l );
                int yhash = this->createHash( previous_elm->geometry.getCentroid().getY() , l );

                // Get envelope
                geos::geom::Envelope env = geos::geom::Envelope(
                            GWSGeometryGetters::getGeometryMinX( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxX( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMinY( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxY( previous_elm->geometry ) );

                this->mutex.lockForWrite();
                this->quadtree_layers.value( l )->value( xhash )->value( yhash )->remove( &env , previous_elm );
                this->quadtree_elements.remove( elm->object_id );
                this->mutex.unlock();

                if( l == 0 ){
                    delete previous_elm;
                }
            }

            if( elm->geometry.isValid() ){

                // Get geohash
                int xhash = this->createHash( elm->geometry.getCentroid().getX() , l );
                int yhash = this->createHash( elm->geometry.getCentroid().getY() , l );

                this->mutex.lockForRead();
                if ( !this->quadtree_layers.value( l )->keys().contains( xhash ) ){
                    this->mutex.unlock();
                    this->mutex.lockForWrite();
                    this->quadtree_layers.value( l )->insert( xhash , new QMap< int , geos::index::quadtree::Quadtree* >() );
                }
                this->mutex.unlock();

                this->mutex.lockForRead();
                if ( !this->quadtree_layers.value( l )->value( xhash )->keys().contains( yhash ) ){
                    this->mutex.unlock();
                    this->mutex.lockForWrite();
                    this->quadtree_layers.value( l )->value( xhash )->insert( yhash , new geos::index::quadtree::Quadtree() );
                }
                this->mutex.unlock();

                // Get envelope
                geos::geom::Envelope env = geos::geom::Envelope(
                            GWSGeometryGetters::getGeometryMinX( elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxX( elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMinY( elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxY( elm->geometry ) );

                this->mutex.lockForWrite();
                this->quadtree_layers.value( l )->value( xhash )->value( yhash )->insert( &env , elm );
                this->quadtree_elements.insert( elm->object_id , elm );
                if( !this->ids_contained.contains( elm->object_id ) ){
                    this->ids_contained.append( elm->object_id );
                }
                this->mutex.unlock();
            }

        });

    }

    /*GWSCoordinate coor = geom.getCentroid();

    if ( coor.isValid() ){

        for( int l = this->layer_amount ; l > 0 ; l-- ){

            QtConcurrent::run([this , coor , l , object_id] {

                if( !this ){ return; }

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
                this->mutex.lockForRead();
                GWSGeometry previous_geom = this->id_to_geometries.value( object_id );
                this->mutex.unlock();
                if( previous_geom.isValid() ){
                    GWSCoordinate previous_coor = previous_geom.getCentroid();
                    int previous_xhash = this->createHash( previous_coor.getX() , l );
                    int previous_yhash = this->createHash( previous_coor.getY() , l );
                    this->mutex.lockForWrite();
                    this->geom_index_layers.value( l )->value( previous_xhash )->value( previous_yhash )->removeAll( object_id );
                    this->mutex.unlock();
                }

                // Insert new version
                this->mutex.lockForWrite();
                this->geom_index_layers.value( l )->value( xhash )->value( yhash )->append( object_id );
                this->mutex.unlock();
            });
        }

        this->mutex.lockForWrite();
        this->id_to_geometries.insert( object_id , geom );
        if( !this->ids_contained.contains( object_id ) ){
            this->ids_contained.append( object_id );
        }
        this->mutex.unlock();
   }*/

}

void GWSQuadtree::remove( QString object_id ){

    if( object_id.isEmpty() ){
        return;
    }

    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements.value( object_id , Q_NULLPTR );
    this->mutex.unlock();

    if( !previous_elm ){ return; }

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

        QtConcurrent::run([this , l , previous_elm] {

            if( previous_elm ){

                // Get geohash
                int xhash = this->createHash( previous_elm->geometry.getCentroid().getX() , l );
                int yhash = this->createHash( previous_elm->geometry.getCentroid().getY() , l );

                // Get envelope
                geos::geom::Envelope env = geos::geom::Envelope(
                            GWSGeometryGetters::getGeometryMinX( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxX( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMinY( previous_elm->geometry ) ,
                            GWSGeometryGetters::getGeometryMaxY( previous_elm->geometry ) );

                this->mutex.lockForWrite();
                this->quadtree_layers.value( l )->value( xhash )->value( yhash )->remove( &env , previous_elm );
                this->quadtree_elements.remove( previous_elm->object_id );
                this->mutex.unlock();

                if( l == 0 ){
                    delete previous_elm;
                }
            }

            this->mutex.lockForWrite();
            this->ids_contained.removeAll( previous_elm->object_id );
            this->mutex.unlock();

        });

    }

    /*
    const GWSGeometry object_geom = this->id_to_geometries.value( object_id );

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
    */
}


int GWSQuadtree::createHash(double value, int decimal_amount) const{
    try {
        double multiplier = qPow( 10 , decimal_amount );
        QString as_str = QString::number( value * multiplier , 'f' , this->layer_depth_amount );
        return (int)as_str.toDouble(); // To avoid rouding made by qFloor
    } catch (std::exception e ){
    }
    return 1;
}
