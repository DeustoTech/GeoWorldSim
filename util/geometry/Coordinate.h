#ifndef GWSCOORDINATE_H
#define GWSCOORDINATE_H

#include "object/Object.h"

class GWSCoordinate : public GWSObject
{
    Q_OBJECT
    friend class GWSGeometryController;
    friend class GWSGraphNode;
    friend class GWSGraphEdge;

public:
    explicit GWSCoordinate();
    GWSCoordinate( double x , double y , double z = 0 );
    GWSCoordinate( const GWSCoordinate& );

    // PROPERTIES
    static QString X_PROP;
    static QString Y_PROP;
    static QString Z_PROP;

    // GETTERS
    double getX() const;
    double getY() const;
    double getZ() const;
    bool isNull() const;
    double distance( GWSCoordinate other ) const;

    // OPERATORS
    bool operator == (const GWSCoordinate&) const;
    bool operator != (const GWSCoordinate&) const;
    GWSCoordinate& operator= (const GWSCoordinate&);

};

Q_DECLARE_METATYPE(GWSCoordinate)

#endif // GWSCOORDINATE_H
