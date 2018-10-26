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
    GWSQuadtree( QString hash );
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


    template <class T = GWSObject> QList< QSharedPointer<T> > getNearestElements( GWSCoordinate coor , unsigned int amount = 1 ){
        QList< QSharedPointer<T> > objects;
        foreach( QSharedPointer<GWSObject> o, this->getNearestElements( coor , amount ) ) {
            if( o ){ objects.append( o.dynamicCast<T>() ); }
        }
        return objects;
    }
    QList< QSharedPointer<GWSObject> > getNearestElements( GWSCoordinate coor , unsigned int amount = 1 );

    template <class T = GWSObject> QList< QSharedPointer<T> > getNearestElements( QSharedPointer<GWSGeometry> geometry , unsigned int amount = 1 ){
        QList< QSharedPointer<T> > objects;
        foreach( QSharedPointer<GWSObject> o, this->getNearestElements( geometry , amount ) ) {
            if( o ){ objects.append( o.dynamicCast<T>() ); }
        }
        return objects;
    }
    QList< QSharedPointer<GWSObject> > getNearestElements( QSharedPointer<GWSGeometry> geometry , unsigned int amount = 1 );

    template <class T = GWSObject> QSharedPointer<T> getNearestElement( GWSCoordinate coor , unsigned int amount = 1 ){
        return this->getNearestElement( coor , amount ).dynamicCast<T>();
    }
    QSharedPointer<GWSObject> getNearestElement( GWSCoordinate coor , unsigned int amount = 1 );

    template <class T = GWSObject> QSharedPointer<T> getNearestElement( QSharedPointer<GWSGeometry> geometry , unsigned int amount = 1 ){
        return this->getNearestElement( geometry , amount ).dynamicCast<T>();
    }
    QSharedPointer<GWSObject> getNearestElement( QSharedPointer<GWSGeometry> geometry );

    // SETTERS
    void upsert( QSharedPointer<GWSObject> object , GWSCoordinate coor );
    void upsert( QSharedPointer<GWSObject> object , QSharedPointer<GWSGeometry> geom );
    void remove( QSharedPointer<GWSObject> object );


protected:

    // HELPER CLASS
    class GWSQuadtreeVisitor : public SpatialIndex::IVisitor {
    public:
        void visitNode(const INode& n) override {}
        void visitData(const IData& d) override { this->visited.append( d.getIdentifier() ); }
        void visitData(std::vector<const IData*>& v) override {}
        QList< SpatialIndex::id_type > visited;
    };

private:

    QMutex mutex;
    //SpatialIndex::ISpatialIndex* inner_index = Q_NULLPTR;
    //QMap< QString , SpatialIndex::id_type > inner_index_ids;
    //QMap< QString , SpatialIndex::Region > inner_index_geometries;
    //quint64 inner_index_last_id = 0;

    QMap< QString , QSharedPointer<GWSObject> > id_to_objects;
    QMap< QString , GWSCoordinate > id_to_coordinates;
    QMap< QString , QMap< QString, QStringList > > geom_index;

};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
