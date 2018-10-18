#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QObject>
#include <QMutex>
#include <QMap>

#include "../../agent/Agent.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"
#include "geos/index/quadtree/Quadtree.h"

using namespace geos::index;

class GWSQuadtree : public QObject
{
    Q_OBJECT

public:
    GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( GWSCoordinate coor ){
        return this->getElements<T>( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( QSharedPointer<GWSGeometry> geom ){
        QList< QSharedPointer<T> > intersecting_agents;
        foreach( QSharedPointer<GWSObject> o , this->getElements<GWSObject>( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() ) ){
            QSharedPointer<GWSGeometry> o_geom = this->id_to_geometries.value( o->getId() );
            if( geom->intersects( o_geom ) ){
                intersecting_agents.append( o.dynamicCast<T>() );
            }
        }
        return intersecting_agents;
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( double minX, double maxX, double minY, double maxY ){
        QList< QSharedPointer<T> > objects;
        foreach (QSharedPointer<GWSObject> o, this->getObjects( minX , maxX , minY , maxY ) ) {
            if( o ){
                objects.append( o.dynamicCast<T>() );
            }
        }
        return objects;
    }

    QList< QSharedPointer<GWSObject> > getObjects( double minX, double maxX, double minY, double maxY );
    QSharedPointer<GWSGeometry> getGeometry( QString object_id );
    QSharedPointer<GWSObject> getNearestElement( GWSCoordinate coor );
    QSharedPointer<GWSObject> getNearestElement( QSharedPointer<GWSGeometry> geometry );

    // SETTERS
    void upsert( QSharedPointer<GWSObject> object , GWSCoordinate coor );
    void upsert( QSharedPointer<GWSObject> object , QSharedPointer<GWSGeometry> geom );
    void remove( QSharedPointer<GWSObject> object );

    // HELPER CLASS
    class GWSQuadtreeElement : public QObject {
    public:
        GWSQuadtreeElement( QString id ) : QObject() , referenced_object_id( id ){}
        const QString referenced_object_id;
    };

private:

    //geos::index::quadtree::Quadtree inner_index;
    QMap< QString , geos::geom::Envelope> id_to_envelopes;
    QMap< QString , QSharedPointer<GWSObject> > id_to_objects;
    //QMap< QString , GWSQuadtreeElement* > id_to_tree_elements;
    QMap< QString , QSharedPointer<GWSGeometry> > id_to_geometries;

};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
