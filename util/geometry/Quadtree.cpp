#include "Quadtree.h"

#include <QDebug>

#include "geos/geom/Envelope.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


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

QList<GWSAgent *> GWSQuadtree::getElements(const GWSGeometry* geometry) const{
    QList<GWSAgent *> intersecting_agents;
    foreach( GWSAgent* a , this->getElements( geometry->getGeometryMinX() , geometry->getGeometryMaxX() , geometry->getGeometryMinY() , geometry->getGeometryMaxY() ) ){
        if( geometry->intersects( GWSPhysicalEnvironment::globalInstance()->getGeometry( a ) ) ){
            intersecting_agents.append( a );
        }
    }
    return intersecting_agents;
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
    GWSLengthUnit found_distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( found )->getCentroid().getDistance( coor );

    for(int i = 0 ; i < agents.size() ; i++){
        GWSAgent* g = agents.at(i);
        if( g ){

            try {
                GWSLengthUnit d = coor.getDistance( GWSPhysicalEnvironment::globalInstance()->getGeometry( g )->getCentroid() );
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
    GWSLengthUnit found_distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( found )->getDistance( geometry );

    for(int i = 0 ; i < agents.size() ; i++){
        GWSAgent* g = agents.at(i);
        if( g ){

            try {
                GWSLengthUnit d = GWSPhysicalEnvironment::globalInstance()->getGeometry( g )->getDistance( geometry );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
}

void GWSQuadtree::upsert(GWSAgent* agent){
    this->mutex.lock();
    // Check if exists
    if( !this->registered_envelopes.value( agent ).isNull() ){
        geos::geom::Envelope e = this->registered_envelopes.value( agent );
        this->inner_index->remove( &e , agent );
    }

    geos::geom::Envelope e = geos::geom::Envelope(
                GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMinX() ,
                GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMaxX() ,
                GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMinY() ,
                GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMaxY() );
    this->registered_envelopes.insert( agent , e );
    this->inner_index->insert( &e , agent );
    this->mutex.unlock();
}

void GWSQuadtree::remove(GWSAgent* agent){
    //this->mutex.lock();
    geos::geom::Envelope e = this->registered_envelopes.value( agent );
    this->inner_index->remove( &e , agent );
    this->registered_envelopes.remove( agent );
    //this->mutex.unlock();
}
