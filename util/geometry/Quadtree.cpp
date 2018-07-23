#include "Quadtree.h"

#include <QDebug>

#include "geos/geom/Envelope.h"

GWSQuadtree::GWSQuadtree() : GWSObject(){
    this->inner_index = new geos::index::quadtree::Quadtree();
}

GWSQuadtree::~GWSQuadtree(){
    delete this->inner_index;
    this->inner_index = 0;
}

QList<GWSAgent *> GWSQuadtree::getElements(GWSPoint point) const{
    /*GWSEnvelope env = GWSEnvelope( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    return this->getElements( env );*/
}

QList<GWSAgent *> GWSQuadtree::getElements(GWSPolygon env) const{
    /*QList<void *> objs;
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
    return objs;*/
}


void GWSQuadtree::insert(GWSAgent* agent){
    /*this->mutex.lock();
    geos::geom::Envelope* e = new geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() );
    this->inner_index->insert( e , elm );
    delete e;
    this->mutex.unlock();*/
}


void GWSQuadtree::remove(GWSAgent* agent){
    /*this->mutex.lock();
    geos::geom::Envelope* e = new geos::geom::Envelope( env.getMinX() , env.getMaxX() , env.getMinY() , env.getMaxY() );
    this->inner_index->remove( e , elm );
    delete e;
    this->mutex.unlock();*/
}
