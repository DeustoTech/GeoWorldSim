#include "Quadtree.h"

#include "GeometryGetters.h"
#include "GeometryTransformators.h"
#include <QJsonArray>
#include <QtConcurrent/QtConcurrent>

GWSQuadtree::GWSQuadtree() : QObject(){
    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){
        this->quadtree_layers.insert( l , new QMap< int , QMap< int, geos::index::quadtree::Quadtree* >* >() );
    }
}

GWSQuadtree::~GWSQuadtree(){
}

QStringList GWSQuadtree::getElements() const {
    return this->ids_contained;
}

const GWSGeometry GWSQuadtree::getBounds() const {
    QJsonObject geom;
    geom.insert( "type" , "Polygon" );
    QJsonArray coors;
    coors.append( QJsonArray({ min_x , min_y }) );
    coors.append( QJsonArray({ min_x , max_y }) );
    coors.append( QJsonArray({ max_x , max_y }) );
    coors.append( QJsonArray({ max_x , min_y }) );
    coors.append( QJsonArray({ min_x , min_y }) );
    QJsonArray rings = { coors };
    geom.insert( "coordinates" , rings );
    return GWSGeometry( geom );
}

const GWSGeometry GWSQuadtree::getGeometry(const QString &object_id ) const {
    QByteArray ba = object_id.toUtf8();
    this->mutex.lockForWrite();
    GWSQuadtreeElement* elm = this->quadtree_elements.value( ba.data() , Q_NULLPTR );
    this->mutex.unlock();

    if( elm ){ return elm->geometry; }
    return GWSGeometry();
}

QStringList GWSQuadtree::getElements( const GWSCoordinate &coor ){
    QStringList objects_ids;

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

        this->mutex.lockForRead();
        if( !this->quadtree_layers.keys().contains( l ) ){
            this->mutex.unlock();
            continue;
        }

        int x_hash = this->createHash( coor.getX() , l );
        int y_hash = this->createHash( coor.getY() , l );

        if( !this->quadtree_layers.value( l )->keys().contains( x_hash ) ){
            this->mutex.unlock();
            continue;
        }

        if( !this->quadtree_layers.value( l )->value( x_hash )->keys().contains( y_hash ) ){
            this->mutex.unlock();
            continue;
        }
        this->mutex.unlock();

        const geos::geom::Envelope env = geos::geom::Envelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
        std::vector<void*> ids;

        this->mutex.lockForWrite();
        this->quadtree_layers.value( l )->value( x_hash )->value( y_hash )->query( &env , ids );
        this->mutex.unlock();
        foreach(void* id, ids) {
            GWSQuadtreeElement* ptr = (GWSQuadtreeElement *)( id );
            if( !ptr ){ continue; }
            objects_ids.append( QString::fromStdString( ptr->object_id ) );
        }

        if( !objects_ids.isEmpty() ){
            return objects_ids;
        }

    }

    return objects_ids;
}

QStringList GWSQuadtree::getElements( const GWSGeometry &geom ){
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
        objects_ids.append( QString::fromStdString( ptr->object_id ) );
    }

    return objects_ids;
}

QString GWSQuadtree::getNearestElement( const GWSCoordinate &coor ) {

    if( !coor.isValid() ){ return QString(); }

    GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
    QString nearest_object_id;
    foreach (QString object_id , this->getElements( coor ) ) {

        this->mutex.lockForRead();
        GWSQuadtreeElement* elm = this->quadtree_elements.value( object_id.toStdString() , Q_NULLPTR );
        this->mutex.unlock();

        if( !elm ){ continue; }
        const GWSGeometry g = elm->geometry;

        if( g.isValid() ){
            GWSLengthUnit l = g.getCentroid().getDistance( coor );
            if( l < shortest_distance ){
                shortest_distance = l;
                nearest_object_id = object_id;
            }
        }
    }

    return nearest_object_id;
}

QString GWSQuadtree::getNearestElement( const GWSGeometry &geom ) {

    if( !geom.isValid() ){ return QString(); }

    GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
    QString nearest_object_id;
    foreach (QString object_id , this->getElements( geom ) ) {

        this->mutex.lockForRead();
        GWSQuadtreeElement* elm = this->quadtree_elements.value( object_id.toStdString() , Q_NULLPTR );
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

void GWSQuadtree::upsert( const QString &object_id , const GWSCoordinate &coor ){
    GWSGeometry geom = GWSGeometry( coor );
    this->upsert( object_id , geom );
}

void GWSQuadtree::upsert( const QString &object_id , const GWSGeometry &geom ){

    if( object_id.isEmpty() ){
        return;
    }

    // Extend the bounds of the index
    GWSCoordinate centroid = geom.getCentroid();
    this->min_x = qMin( centroid.getX() , min_x );
    this->max_x = qMax( centroid.getX() , max_x );
    this->min_y = qMin( centroid.getY() , min_y );
    this->max_y = qMax( centroid.getY() , max_y );

    // Create helper element
    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements.value( object_id.toStdString() , Q_NULLPTR );
    this->mutex.unlock();

    GWSQuadtreeElement* elm = new GWSQuadtreeElement( object_id.toStdString() , geom );

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

        QtConcurrent::run([this , l , elm , previous_elm] {

            if( previous_elm ){

                GWSGeometry previous_elm_geom = previous_elm->geometry;
                if( previous_elm_geom.isValid() ){

                    // Get geohash
                    int xhash = this->createHash( previous_elm_geom.getCentroid().getX() , l );
                    int yhash = this->createHash( previous_elm_geom.getCentroid().getY() , l );

                    // Get envelope
                    geos::geom::Envelope env = geos::geom::Envelope(
                                GWSGeometryGetters::getGeometryMinX( previous_elm_geom ) ,
                                GWSGeometryGetters::getGeometryMaxX( previous_elm_geom ) ,
                                GWSGeometryGetters::getGeometryMinY( previous_elm_geom ) ,
                                GWSGeometryGetters::getGeometryMaxY( previous_elm_geom ) );

                    this->mutex.lockForWrite();
                    this->quadtree_layers.value( l )->value( xhash )->value( yhash )->remove( &env , previous_elm );
                    this->quadtree_elements.remove( elm->object_id );
                    this->mutex.unlock();

                }

                if( l == 0 ){
                    delete previous_elm;
                }
            }

            GWSGeometry elm_geom = elm->geometry;

            if( elm_geom.isValid() ){

                // Get geohash
                GWSCoordinate centroid = elm_geom.getCentroid();
                int xhash = this->createHash( centroid.getX() , l );
                int yhash = this->createHash( centroid.getY() , l );

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
                            GWSGeometryGetters::getGeometryMinX( elm_geom ) ,
                            GWSGeometryGetters::getGeometryMaxX( elm_geom ) ,
                            GWSGeometryGetters::getGeometryMinY( elm_geom ) ,
                            GWSGeometryGetters::getGeometryMaxY( elm_geom ) );

                this->mutex.lockForWrite();
                this->quadtree_layers.value( l )->value( xhash )->value( yhash )->insert( &env , elm );
                this->quadtree_elements.insert( elm->object_id , elm );
                QString qstring = QString::fromStdString( elm->object_id );
                if( !this->ids_contained.contains( qstring ) ){
                    this->ids_contained.append( qstring );
                }
                this->mutex.unlock();
            }

        });

    }

}

void GWSQuadtree::remove( const QString &object_id ){

    if( object_id.isEmpty() ){
        return;
    }

    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements.value( object_id.toStdString() , Q_NULLPTR );
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

            QString qstring = QString::fromStdString( previous_elm->object_id );
            this->mutex.lockForWrite();
            this->ids_contained.removeAll( qstring );
            this->mutex.unlock();

        });

    }

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
