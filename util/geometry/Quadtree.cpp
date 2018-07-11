#include "Quadtree.h"

#include <QDebug>

#include "geos/geom/Envelope.h"

GWSQuadtree::GWSQuadtree(QObject *parent) : QObject(parent){
    this->inner_index = new geos::index::quadtree::Quadtree();
}

GWSQuadtree::~GWSQuadtree(){
    delete this->inner_index;
    this->inner_index = 0;
}

QList<void *> GWSQuadtree::getElements(GWSCoordinate coor) const{
    GWSEnvelope env = GWSEnvelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    return this->getElements( env );
}

QList<void *> GWSQuadtree::getElements(GWSEnvelope env) const{
    QList<void *> objs;
    std::vector<void*> vector;

    if( !this->inner_index ){
        qWarning() << "Quadtree has no inner index";
        return objs;
    }

    geos::geom::Envelope* e = new geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() );
    this->inner_index->query( e , vector );
    delete e;

    if( vector.size() ){
        for(unsigned int i = 0 ; i < vector.size() ; i++){
            objs.append( vector.at(i) );
        }
    }
    return objs;
}

void GWSQuadtree::insert(GWSCoordinate env, void *elm){
    this->insert( GWSEnvelope( env.getX() , env.getX() , env.getY() , env.getY() ) , elm );
}

void GWSQuadtree::insert(GWSEnvelope env, void *elm){
    this->mutex.lock();
    geos::geom::Envelope* e = new geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() );
    this->inner_index->insert( e , elm );
    delete e;
    this->mutex.unlock();
}

void GWSQuadtree::remove(GWSCoordinate env, void *elm){
    this->remove( GWSEnvelope( env.getX() , env.getX() , env.getY() , env.getY() ) , elm );
}

void GWSQuadtree::remove(GWSEnvelope env , void *elm){
    this->mutex.lock();
    geos::geom::Envelope* e = new geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() );
    this->inner_index->remove( e , elm );
    delete e;
    this->mutex.unlock();
}
