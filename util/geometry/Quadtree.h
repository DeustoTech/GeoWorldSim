#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>

#include <geos/index/quadtree/Quadtree.h>

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
    const GWSGeometry getGeometry( const QString &object_id ) const;

    QStringList getElements( const GWSCoordinate &coordinate );
    QStringList getElements( const GWSGeometry &geom );
    QStringList getElements( double minX, double maxX, double minY, double maxY );

    QString getNearestElement( const GWSCoordinate &coor );
    QString getNearestElement( const GWSGeometry &geometry );


signals:

    void upsertGeometrySignal( const QString &object_id , const GWSGeometry &coor );
    void upsertCoordinateSignal( const QString &object_id , const GWSCoordinate &coor );
    void removeSignal( const QString &object_id );

public slots:

    // SETTERS
    void upsertCoordinate( const QString &object_id , const GWSCoordinate &coor );
    void upsertGeometry( const QString &object_id , const GWSGeometry &geom );
    void remove( const QString &object_id );

protected:

    std::string createHash( double value , int decimal_amount ) const;

private:

    // HELPER CLASS
    class GWSQuadtreeElement {
    public:
        GWSQuadtreeElement( std::string object_id , const GWSGeometry &geometry ) : object_id( object_id ) , geometry( geometry ) {}
        std::string object_id;
        GWSGeometry geometry;
        geos::geom::Envelope envelope;
    };

    mutable QReadWriteLock mutex;

    QStringList ids_contained;
    QMap< std::string , GWSQuadtree::GWSQuadtreeElement* >* quadtree_elements; // QMAP< (XHASH + YHASH) , QuadTreeElement >
    QMap< std::string , geos::index::quadtree::Quadtree* >* quadtree_layers; // QMAP< (XHASH + YHASH) , QuadTree >
    const unsigned int layer_depth_amount = 5;
    int stored_amount = 0;

    // BOUNDS
    double min_x = 180;
    double max_x = -180;
    double min_y = 180;
    double max_y = -180;

};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
