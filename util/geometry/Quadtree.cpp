#include "Quadtree.h"

#include <QJsonArray>

#include "GeometryGetters.h"
#include "GeometryTransformators.h"

#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../parallelism/ParallelismController.h"

GWSQuadtree::GWSQuadtree() : QObject(){
    this->quadtree_elements = new QMap< std::string , GWSQuadtree::GWSQuadtreeElement* >();
    this->quadtree_layers = new QMap< std::string , geos::index::quadtree::Quadtree* >();

    this->connect( this , &GWSQuadtree::upsertGeometrySignal , this , &GWSQuadtree::upsertGeometry );
    this->connect( this , &GWSQuadtree::upsertCoordinateSignal , this , &GWSQuadtree::upsertGeometry );
    this->connect( this , &GWSQuadtree::removeSignal , this , &GWSQuadtree::remove );
}

GWSQuadtree::~GWSQuadtree(){
    foreach( std::string l , this->quadtree_layers->keys() ){
        delete this->quadtree_layers->value( l );
    }
    delete this->quadtree_layers;
    foreach( std::string l , this->quadtree_elements->keys() ) {
        delete this->quadtree_elements->value( l );
    }
    delete this->quadtree_elements;
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
    this->mutex.lockForWrite();
    GWSQuadtreeElement* elm = this->quadtree_elements->value( object_id.toStdString() , Q_NULLPTR );
    this->mutex.unlock();

    if( elm ){ return elm->geometry; }
    return GWSGeometry();
}

QStringList GWSQuadtree::getElements( const GWSCoordinate &coor ){
    QStringList objects_ids;

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

        std::string combined_hash = this->createHash( coor.getX() , l ) + ":" + this->createHash( coor.getY() , l );

        this->mutex.lockForRead();
        if( !this->quadtree_layers->value( combined_hash , Q_NULLPTR ) ){
            this->mutex.unlock();
            continue;
        }
        this->mutex.unlock();

        std::vector<void*> ids;

        this->mutex.lockForWrite();
        const geos::geom::Envelope env = geos::geom::Envelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
        this->quadtree_layers->value( combined_hash )->query( &env , ids );
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

    std::string combined_hash = this->createHash( ( minX + maxX ) / 2.0  , qMin( x_decimal_count , y_decimal_count ) ) + ":" + this->createHash( ( minY + maxY ) / 2.0 , qMin( x_decimal_count , y_decimal_count ) );

    this->mutex.lockForRead();
    if( !this->quadtree_layers->value( combined_hash , Q_NULLPTR ) ){
        this->mutex.unlock();
        return objects_ids;
    }
    this->mutex.unlock();

    const geos::geom::Envelope env = geos::geom::Envelope( minX , maxX , minY , maxY );
    std::vector<void*> ids;

    this->mutex.lockForWrite();
    this->quadtree_layers->value( combined_hash )->query( &env , ids );
    this->mutex.unlock();

    foreach(void* id, ids) {
        GWSQuadtreeElement* ptr = (GWSQuadtreeElement *)( id );
        if( !ptr ){ continue; }
        objects_ids.append( QString::fromStdString( ptr->object_id ) );
    }

    return objects_ids;
}

QString GWSQuadtree::getNearestElement( const GWSCoordinate &coor ) {

    if( !coor.isValid() ){
        qDebug() << "Invalid coordinates" << coor.toString();
        return QString();
    }

    GWSLengthUnit shortest_distance = GWSLengthUnit(999999999999999);
    QString nearest_object_id;
    foreach (QString object_id , this->getElements( coor ) ) {

        this->mutex.lockForRead();
        GWSQuadtreeElement* elm = this->quadtree_elements->value( object_id.toStdString() , Q_NULLPTR );
        this->mutex.unlock();

        if( !elm ){ continue; }
        const GWSGeometry& g = elm->geometry;

        if( g.isValid() ){
            const GWSLengthUnit& l = g.getCentroid().getDistance( coor );
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

    GWSLengthUnit shortest_distance = 999999999999999;
    QString nearest_object_id;
    foreach (QString object_id , this->getElements( geom ) ) {

        this->mutex.lockForRead();
        GWSQuadtreeElement* elm = this->quadtree_elements->value( object_id.toStdString() , Q_NULLPTR );
        this->mutex.unlock();

        if( !elm ){ continue; }
        const GWSGeometry& g = elm->geometry;

        if( g.isValid() ){
            const GWSLengthUnit& l = g.getCentroid().getDistance( geom.getCentroid() );
            if( l < shortest_distance ){
                shortest_distance = l;
                nearest_object_id = object_id;
            }
        }
    }

    return nearest_object_id;
}

void GWSQuadtree::upsertCoordinate( const QString &object_id , const GWSCoordinate &coor ){
    const GWSGeometry& geom( coor );
    this->upsertGeometry( object_id , geom );
}

void GWSQuadtree::upsertGeometry( const QString &object_id , const GWSGeometry &geom ){

    if( object_id.isEmpty() ){
        return;
    }

    // Extend the bounds of the index
    const GWSCoordinate& centroid = geom.getCentroid();
    this->min_x = qMin( centroid.getX() , min_x );
    this->max_x = qMax( centroid.getX() , max_x );
    this->min_y = qMin( centroid.getY() , min_y );
    this->max_y = qMax( centroid.getY() , max_y );

    // Create helper element
    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements->value( object_id.toStdString() , Q_NULLPTR );
    this->mutex.unlock();

    this->stored_amount++;
    if( stored_amount % 1000 == 0 ){
        QString message = QString("Quadtree indexing %1 entities" ).arg( stored_amount );
        qInfo() << message;
        emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal( QJsonObject({ { "message" , message } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );
    }

    GWSQuadtreeElement* elm = new GWSQuadtreeElement( object_id.toStdString() , geom );

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

            if( previous_elm ){

                const GWSGeometry& previous_elm_geom = previous_elm->geometry;
                if( previous_elm_geom.isValid() ){

                    // Get geohash
                    std::string total_hash = this->createHash( previous_elm_geom.getCentroid().getX() , l ) + ":" + this->createHash( previous_elm_geom.getCentroid().getY() , l );

                    this->mutex.lockForWrite();
                    this->quadtree_layers->value( total_hash )->remove( &previous_elm->envelope , previous_elm );
                    this->quadtree_elements->remove( elm->object_id );
                    this->mutex.unlock();

                }

                if( l == 0 ){
                    delete previous_elm;
                }
            }

            const GWSGeometry& elm_geom = elm->geometry;

            if( elm_geom.isValid() ){

                // Get geohash
                GWSCoordinate centroid = elm_geom.getCentroid();
                std::string combined_hash = this->createHash( centroid.getX() , l ) + ":" + this->createHash( centroid.getY() , l );

                this->mutex.lockForRead();
                geos::index::quadtree::Quadtree* tree = this->quadtree_layers->value( combined_hash , Q_NULLPTR );

                if ( !tree ){
                    this->mutex.unlock();

                    this->mutex.lockForWrite();
                    tree = new geos::index::quadtree::Quadtree();
                    this->quadtree_layers->insert( combined_hash , tree );
                }
                this->mutex.unlock();


                this->mutex.lockForWrite();

                try {
                    // Get envelope
                    elm->envelope = geos::geom::Envelope(
                                GWSGeometryGetters::getGeometryMinX( elm_geom ) ,
                                GWSGeometryGetters::getGeometryMaxX( elm_geom ) ,
                                GWSGeometryGetters::getGeometryMinY( elm_geom ) ,
                                GWSGeometryGetters::getGeometryMaxY( elm_geom ) );

                    tree->insert( &elm->envelope , elm );
                } catch( ... ){}

                this->quadtree_elements->insert( elm->object_id , elm );
                QString qstring = QString::fromStdString( elm->object_id );
                if( !this->ids_contained.contains( qstring ) ){
                    this->ids_contained.append( qstring );
                }
                this->mutex.unlock();
            }
    }

}

void GWSQuadtree::remove( const QString &object_id ){

    if( object_id.isEmpty() ){
        return;
    }

    this->mutex.lockForRead();
    GWSQuadtreeElement* previous_elm = this->quadtree_elements->value( object_id.toStdString() , Q_NULLPTR );
    this->mutex.unlock();

    if( !previous_elm ){ return; }

    for( int l = this->layer_depth_amount ; l > 0 ; l-- ){

            if( previous_elm ){

                // Get geohash
                std::string combined_hash = this->createHash( previous_elm->geometry.getCentroid().getX() , l ) + ":" + this->createHash( previous_elm->geometry.getCentroid().getY() , l );

                this->mutex.lockForWrite();

                this->quadtree_layers->value( combined_hash )->remove( &previous_elm->envelope , previous_elm );
                this->quadtree_elements->remove( previous_elm->object_id );
                this->mutex.unlock();
            }

            if( l == 0 ){
                delete previous_elm;
            }

    }

    QString qstring = QString::fromStdString( previous_elm->object_id );
    this->mutex.lockForWrite();
    this->ids_contained.removeAll( qstring );
    this->mutex.unlock();

}


std::string GWSQuadtree::createHash(double value, int decimal_amount) const{
    try {
        double multiplier = qPow( 10 , decimal_amount );
        int trimmed = value * multiplier;
        return QString::number( trimmed ).toStdString();
    } catch (std::exception e ){
    }
    return "";
}
