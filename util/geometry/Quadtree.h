#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>

#include <geos/index/quadtree/Quadtree.h>

#include "../../agent/Agent.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"

using namespace geos::index::quadtree;

class GWSQuadtree : public QObject
{
    Q_OBJECT

public:
    GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    QStringList getElements() const;
    const GWSGeometry getBounds() const;
    const GWSGeometry getGeometry( QString object_id ) const;

    QStringList getElements( const GWSCoordinate coordinate );
    QStringList getElements( const GWSGeometry geom );
    QStringList getElements( double minX, double maxX, double minY, double maxY );

    QString getNearestElement( GWSCoordinate coor );
    QString getNearestElement( GWSGeometry geometry );

    // SETTERS
    void upsert( QString object_id , GWSCoordinate coor );
    void upsert( QString object_id , const GWSGeometry geom );
    void remove( QString object_id );

protected:

    int createHash( double value , int decimal_amount ) const;

private:

    // HELPER CLASS
    class GWSQuadtreeElement {
    public:
        GWSQuadtreeElement( QString object_id , GWSGeometry geometry ) : object_id( object_id ) , geometry( geometry ) {}
        QString object_id;
        GWSGeometry geometry;
    };

    mutable QReadWriteLock mutex;

    QStringList ids_contained;
    QMap< QString , GWSQuadtree::GWSQuadtreeElement* > quadtree_elements;
    QMap< unsigned int , QMap< int , QMap< int , geos::index::quadtree::Quadtree* >* >* > quadtree_layers;
    unsigned int layer_depth_amount = 5;
    GWSGeometry index_bounds;

};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
