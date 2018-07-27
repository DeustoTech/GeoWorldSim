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

QList<GWSAgent *> GWSQuadtree::getElements(GWSCoordinate coor) const{
    return this->getElements( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
}

QList<GWSAgent *> GWSQuadtree::getElements(GWSGeometry* geometry) const{
    return this->getElements( geometry->getGeometryMinX() , geometry->getGeometryMaxX() , geometry->getGeometryMinY() , geometry->getGeometryMaxY() );
}

QList<GWSAgent *> GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) const{
    QList<GWSAgent *> agents;
    std::vector<void*> vector;

    if( !this->inner_index ){
        qWarning() << "Quadtree has no inner index";
        return agents;
    }

    geos::geom::Envelope* e = new geos::geom::Envelope( minX , maxX , minY , maxY );
    this->inner_index->query( e , vector );
    delete e;

    if( vector.size() ){
        for(unsigned int i = 0 ; i < vector.size() ; i++){
            agents.append( (GWSAgent*)vector.at(i) );
        }
    }
    return agents;
}

GWSAgent* GWSQuadtree::getNearestElement(GWSCoordinate coor) const{
    GWSAgent* found = Q_NULLPTR;
    QList<GWSAgent*> agents = this->getElements( coor );

    if( agents.isEmpty() ){
        return found;
    }

    found = agents.at( 0 );
    GWSLengthUnit found_distance = found->getCentroid().getDistance( coor );

    for(int i = 0 ; i < agents.size() ; i++){
        GWSAgent* g = agents.at(i);
        if( g ){

            try {
                GWSLengthUnit d = coor.getDistance( g->getCentroid() );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
}

GWSAgent* GWSQuadtree::getNearestElement(GWSGeometry *geometry) const{

    GWSAgent* found = Q_NULLPTR;
    QList<GWSAgent*> agents = this->getElements( geometry );

    if( agents.isEmpty() ){
        return found;
    }

    found = agents.at( 0 );
    GWSLengthUnit found_distance = found->getDistance( geometry );

    for(int i = 0 ; i < agents.size() ; i++){
        GWSAgent* g = agents.at(i);
        if( g ){

            try {
                GWSLengthUnit d = geometry->getDistance( g );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
}

void GWSQuadtree::insert(GWSAgent* agent){
    //this->mutex.lock();
    if( const GWSGeometry* geom = agent ){
        geos::geom::Envelope* e = new geos::geom::Envelope( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() );
        this->inner_index->insert( e , agent );
        delete e;
    }
    //this->mutex.unlock();
}


void GWSQuadtree::remove(GWSAgent* agent){
    //this->mutex.lock();
    if( const GWSGeometry* geom = agent ){
        geos::geom::Envelope* e = new geos::geom::Envelope( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() );
        this->inner_index->remove( e , agent );
        delete e;
    }
    //this->mutex.unlock();
}
