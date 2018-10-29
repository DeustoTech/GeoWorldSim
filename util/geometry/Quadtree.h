#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <spatialindex/SpatialIndex.h>
#include <spatialindex/Region.h>
#include <spatialindex/RTree.h>

#include "../../agent/Agent.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;
using namespace SpatialIndex::RTree;

class GWSQuadtree : public QObject
{
    Q_OBJECT

public:
    GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements(){
        QList< QSharedPointer<T> > objects;
        foreach( QSharedPointer<GWSObject> o, this->getElements() ) {
            if( o ){ objects.append( o.dynamicCast<T>() ); }
        }
        return objects;
    }
    QList< QSharedPointer<GWSObject> > getElements();
    QSharedPointer<GWSGeometry> getGeometry( QString object_id );

    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( GWSCoordinate coor ){
        return this->getElements<T>( coor.getX() , coor.getX() , coor.getY() , coor.getY() );
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( QSharedPointer<GWSGeometry> geom ){
        return this->getElements<T>( geom->getGeometryMinX() , geom->getGeometryMaxX() , geom->getGeometryMinY() , geom->getGeometryMaxY() );
    }
    template <class T = GWSObject> QList< QSharedPointer<T> > getElements( double minX, double maxX, double minY, double maxY ){
        QList< QSharedPointer<T> > objects;
        foreach( QSharedPointer<GWSObject> o, this->getElements( minX , maxX , minY , maxY ) ) {
            if( o ){ objects.append( o.dynamicCast<T>() ); }
        }
        return objects;
    }
    QList< QSharedPointer<GWSObject> > getElements( double minX, double maxX, double minY, double maxY );

    QSharedPointer<GWSObject> getNearestElement( GWSCoordinate coor );
    QSharedPointer<GWSObject> getNearestElement( QSharedPointer<GWSGeometry> geometry );

    template <class T = GWSObject> QSharedPointer<T> getNearestElement( GWSCoordinate coor ){
        return this->getNearestElement( coor ).dynamicCast<T>();
    }
    template <class T = GWSObject> QSharedPointer<T> getNearestElements( QSharedPointer<GWSGeometry> geometry ){
        return this->getNearestElement<T>( geometry->getCentroid() );
    }

    // SETTERS
    void upsert( QSharedPointer<GWSObject> object , GWSCoordinate coor );
    void upsert( QSharedPointer<GWSObject> object , QSharedPointer<GWSGeometry> geom );
    void remove( QSharedPointer<GWSObject> object );

protected:

    int createHash( double value , int decimal_amount );

private:

    QMutex mutex;

    QMap< QString , QSharedPointer<GWSObject> > id_to_objects;
    QMap< QString , QSharedPointer<GWSGeometry> > id_to_geometries;
    QMap< unsigned int , QMap< int , QMap< int , QStringList* >* >* > geom_index_layers;
    unsigned int layer_amount = 5;
    QList< GWSCoordinate > fast_compare_if_layers_created;

};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
