#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QMutex>
#include <QMap>

#include "../../object/Object.h"
#include "../../agent/Agent.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"
#include "geos/index/quadtree/Quadtree.h"

using namespace geos::index;

class GWSQuadtree : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( GWSCoordinate coor ) const{
        return this->getElements<T>( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( QSharedPointer<GWSGeometry> geom ) const{
        QList< QSharedPointer<T> > intersecting_agents;
        foreach( QSharedPointer<GWSObject> o , this->getElements<GWSObject>( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() ) ){
            QSharedPointer<GWSGeometry> o_geom = this->id_to_geometries.value( o->getId() );
            if( geom->intersects( o_geom ) ){
                intersecting_agents.append( o.dynamicCast<T>() );
            }
        }
        return intersecting_agents;
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( double minX, double maxX, double minY, double maxY ) const{
        QList< QSharedPointer<T> > objects;
        foreach (QSharedPointer<GWSObject> o, this->getObjects( minX , maxX , minY , maxY ) ) {
            objects.append( o.dynamicCast<T>() );
        }
        return objects;
    }

    QList< QSharedPointer<GWSObject> > getObjects( double minX, double maxX, double minY, double maxY ) const;

    QSharedPointer<GWSObject> getNearestElement( GWSCoordinate coor ) const;
    QSharedPointer<GWSObject> getNearestElement( QSharedPointer<GWSGeometry> geometry ) const;

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

    geos::index::quadtree::Quadtree* inner_index;
    QMap< QString , geos::geom::Envelope> registered_envelopes;
    QMap< QString , QSharedPointer<GWSObject> > id_to_objects;
    QMap< QString , GWSQuadtreeElement* > id_to_tree_elements;
    QMap< QString , QSharedPointer<GWSGeometry> > id_to_geometries;

    QMutex mutex;
};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
