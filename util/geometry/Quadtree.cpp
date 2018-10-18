#include "Quadtree.h"

#include <QDebug>

#include "geos/geom/Envelope.h"


GWSQuadtree::GWSQuadtree() : QObject(){
}

GWSQuadtree::~GWSQuadtree(){
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

QList< QSharedPointer<GWSObject> > GWSQuadtree::getObjects(double minX, double maxX, double minY, double maxY) {
    QList< QSharedPointer<GWSObject> > objects;

    geos::geom::Envelope e = geos::geom::Envelope( minX , maxX , minY , maxY );
    foreach( QString object_id , this->id_to_envelopes.keys() ){
        if( e.intersects( this->id_to_envelopes.value( object_id ) ) ){
            objects.append( this->id_to_objects.value( object_id ) );
        }
    }

    if( objects.isEmpty() ){
        return this->id_to_objects.values(); // Return all values, no object could be found inside given bounds
    }

    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor) {
    QSharedPointer<GWSObject> found;
    QList< QSharedPointer<GWSObject> > objects = this->getElements<GWSObject>( coor );

    if( objects.isEmpty() ){
        return found;
    }

    found = objects.at( 0 );
    QSharedPointer<GWSGeometry> found_geom = this->id_to_geometries.value( found->getId() );
    GWSLengthUnit found_distance = found_geom->getCentroid().getDistance( coor );

    for(int i = 0 ; i < objects.size() ; i++){
        QSharedPointer<GWSObject> g = objects.at(i);
        QSharedPointer<GWSGeometry> g_geom = this->id_to_geometries.value( g->getId() );
        if( g && g_geom ){

            try {
                GWSLengthUnit d = g_geom->getCentroid().getDistance( coor );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(QSharedPointer<GWSGeometry> geometry) {

    QSharedPointer<GWSObject> found = Q_NULLPTR;
    QList< QSharedPointer<GWSObject> > agents = this->getElements<GWSObject>( geometry );

    if( agents.isEmpty() ){
        return found;
    }

    found = agents.at( 0 );
    QSharedPointer<GWSGeometry> found_geom = this->id_to_geometries.value( found->getId() );
    GWSLengthUnit found_distance = found_geom->getDistance( geometry );

    for(int i = 0 ; i < agents.size() ; i++){
        QSharedPointer<GWSObject> g = agents.at(i);
        QSharedPointer<GWSGeometry> g_geom = this->id_to_geometries.value( g->getId() );
        if( g && g_geom ){

            try {
                GWSLengthUnit d = g_geom->getDistance( geometry );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
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

        geos::geom::Envelope e = geos::geom::Envelope(
                    geom->getGeometryMinX() ,
                    geom->getGeometryMaxX() ,
                    geom->getGeometryMinY() ,
                    geom->getGeometryMaxY() );

        this->id_to_objects.insert( object_id , object );
        this->id_to_envelopes.insert( object_id , e );
        this->id_to_geometries.insert( object_id , geom );
    }

}

void GWSQuadtree::remove(QSharedPointer<GWSObject> object){

    if( object.isNull() ){
        return;
    }

    QString object_id = object->getId();

    // Check if exists
    if( !this->id_to_envelopes.keys().contains( object_id ) ){
        return;
    }

    this->id_to_envelopes.remove( object_id );
    this->id_to_objects.remove( object_id );
    this->id_to_geometries.remove( object_id );
}
