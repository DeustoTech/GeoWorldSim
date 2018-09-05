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

QList< QSharedPointer<GWSAgent> > GWSQuadtree::getElements(GWSCoordinate coor) const{
    return this->getElements( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
}

QList< QSharedPointer<GWSAgent> > GWSQuadtree::getElements(QSharedPointer<GWSGeometry> geometry) const{
    QList< QSharedPointer<GWSAgent> > intersecting_agents;
    foreach( QSharedPointer<GWSAgent> a , this->getElements( geometry->getGeometryMinX() , geometry->getGeometryMaxX() , geometry->getGeometryMinY() , geometry->getGeometryMaxY() ) ){
        if( geometry->intersects( GWSPhysicalEnvironment::globalInstance()->getGeometry( a ) ) ){
            intersecting_agents.append( a );
        }
    }
    return intersecting_agents;
}

QList< QSharedPointer<GWSAgent> > GWSQuadtree::getElements(double minX, double maxX, double minY, double maxY) const{
    QList< QSharedPointer<GWSAgent> > agents;
    std::vector<void*> vector;

    if( !this->inner_index ){
        qWarning() << "Quadtree has no inner index";
        return agents;
    }

    geos::geom::Envelope* e = new geos::geom::Envelope( minX , maxX , minY , maxY );
    this->inner_index->query( e , vector );
    delete e;

    if( vector.empty() ){
        vector = *this->inner_index->queryAll();
    }

    if( vector.size() ){
        for(unsigned int i = 0 ; i < vector.size() ; i++){
            GWSAgent* a = (GWSAgent*)vector.at(i);
            agents.append( a->getSharedPointer() );
        }
    }

    return agents;
}

QSharedPointer<GWSAgent> GWSQuadtree::getNearestElement(GWSCoordinate coor) const{
    QSharedPointer<GWSAgent> found;
    QList< QSharedPointer<GWSAgent> > agents = this->getElements( coor );

    if( agents.isEmpty() ){
        return found;
    }

    found = agents.at( 0 );
    GWSLengthUnit found_distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( found )->getCentroid().getDistance( coor );

    for(int i = 0 ; i < agents.size() ; i++){
        QSharedPointer<GWSAgent> g = agents.at(i);
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

QSharedPointer<GWSAgent> GWSQuadtree::getNearestElement(QSharedPointer<GWSGeometry> geometry) const{

    QSharedPointer<GWSAgent> found = Q_NULLPTR;
    QList< QSharedPointer<GWSAgent> > agents = this->getElements( geometry );

    if( agents.isEmpty() ){
        return found;
    }

    found = agents.at( 0 );
    GWSLengthUnit found_distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( found )->getDistance( geometry );

    for(int i = 0 ; i < agents.size() ; i++){
        QSharedPointer<GWSAgent> g = agents.at(i);
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

void GWSQuadtree::upsert( QSharedPointer<GWSAgent> agent , GWSCoordinate coor ){
    QSharedPointer<GWSGeometry> geom = QSharedPointer<GWSGeometry>( new GWSGeometry() );
    geom->transformMove( coor );
    this->upsert( agent , geom );
    geom.clear();
}

void GWSQuadtree::upsert( QSharedPointer<GWSAgent> agent , QSharedPointer<GWSGeometry> geom ){
    this->mutex.lock();
    // Check if exists
    if( !this->registered_envelopes.value( agent ).isNull() ){
        geos::geom::Envelope e = this->registered_envelopes.value( agent );
        this->inner_index->remove( &e , agent.data() );
    }

    if( geom ){
        geos::geom::Envelope e = geos::geom::Envelope(
                    geom->getGeometryMinX() ,
                    geom->getGeometryMaxX() ,
                    geom->getGeometryMinY() ,
                    geom->getGeometryMaxY() );
        this->registered_envelopes.insert( agent , e );
        this->inner_index->insert( &e , agent.data() );
    }
    this->mutex.unlock();
}

void GWSQuadtree::remove(QSharedPointer<GWSAgent> agent){
    //this->mutex.lock();
    geos::geom::Envelope e = this->registered_envelopes.value( agent );
    this->inner_index->remove( &e , agent.data() );
    this->registered_envelopes.remove( agent );
    //this->mutex.unlock();
}
