#include "Quadtree.h"

#include <QDebug>

#include "geos/geom/Envelope.h"


GWSQuadtree::GWSQuadtree() : GWSObject(){
    this->inner_index = new geos::index::quadtree::Quadtree();
}

GWSQuadtree::~GWSQuadtree(){
    delete this->inner_index;
    this->inner_index = Q_NULLPTR;
}

/*QList< QSharedPointer<GWSObject> > GWSQuadtree::getElements( GWSCoordinate coor ) const{
    return this->getElements<GWSObject>( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
}

template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements( GWSCoordinate coor ) const{

}

template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements( QSharedPointer<GWSGeometry> geom ) const{
    QList< QSharedPointer<T> > intersecting_agents;
    foreach( QSharedPointer<GWSObject> o , this->getElements<GWSObject>( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() ) ){
        QSharedPointer<GWSGeometry> o_geom = this->id_to_geometries.value( o->getId() );
        if( geom->intersects( o_geom ) ){
            intersecting_agents.append( o.dynamicCast<T>() );
        }
    }
    return intersecting_agents;
}


template <class T> QList< QSharedPointer<T> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) const{

}*/

QList< QSharedPointer<GWSObject> > GWSQuadtree::getObjects(double minX, double maxX, double minY, double maxY) const{
    QList< QSharedPointer<GWSObject> > objects;
    std::vector<void*> vector;

    if( !this->inner_index ){
        qWarning() << "Quadtree has no inner index";
        return objects;
    }

    geos::geom::Envelope* e = new geos::geom::Envelope( minX , maxX , minY , maxY );
    this->mutex.lock();
    this->inner_index->query( e , vector );
    this->mutex.unlock();
    delete e;

    if( vector.empty() ){
        qDebug() << "Empty vector";
        vector = *this->inner_index->queryAll();
    }

    if( vector.size() ){
        for(unsigned int i = 0 ; i < vector.size() ; i++){
            GWSQuadtreeElement* elm = (GWSQuadtreeElement*)vector.at(i);
            if( elm ){
                QSharedPointer<GWSObject> obj = this->id_to_objects.value( elm->referenced_object_id );
                if( obj ){
                   objects.append( obj );
                }
            }
        }
    }

    return objects;
}

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(GWSCoordinate coor) const{
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

QSharedPointer<GWSObject> GWSQuadtree::getNearestElement(QSharedPointer<GWSGeometry> geometry) const{

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

        GWSQuadtreeElement* elm = Q_NULLPTR;
        geos::geom::Envelope e;

        // Check if exists
        if( !( e = this->id_to_envelopes.value( object_id ) ).isNull() ){

            // Exists
            elm = this->id_to_tree_elements.value( object_id );
            this->inner_index->remove( &e , elm );      // Remove from last position

        } else {

            // Create for first time
            elm = new GWSQuadtreeElement( object_id );
            this->id_to_tree_elements.insert( object_id , elm );
            this->id_to_objects.insert( object_id , object );

        }

        e = geos::geom::Envelope(
                    geom->getGeometryMinX() ,
                    geom->getGeometryMaxX() ,
                    geom->getGeometryMinY() ,
                    geom->getGeometryMaxY() );

        this->id_to_envelopes.insert( object_id , e );
        this->id_to_geometries.insert( object_id , geom );
        this->mutex.lock();
        this->inner_index->insert( &e , elm );          // Insert in current position
        this->mutex.unlock();
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

    this->mutex.lock();
    geos::geom::Envelope e = this->id_to_envelopes.value( object_id );
    GWSQuadtreeElement* elm = this->id_to_tree_elements.value( object_id );
    this->inner_index->remove( &e , elm );
    this->id_to_envelopes.remove( object_id );
    this->id_to_objects.remove( object_id );
    this->id_to_tree_elements.remove( object_id );
    this->id_to_geometries.remove( object_id );
    elm->deleteLater();
    this->mutex.unlock();

}
