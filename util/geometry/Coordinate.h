#ifndef GWSCOORDINATE_H
#define GWSCOORDINATE_H

#include "../../object/Object.h"


/**
 * @brief The GeoCoordinates class
 * @url https://schema.org/GeoCoordinates
 */
class GWSCoordinate : public GWSObject
{
    Q_OBJECT
    friend class GWSGeometryController;
    friend class GWSGraphNode;
    friend class GWSGraphEdge;

public:
    Q_INVOKABLE explicit GWSCoordinate( QObject* parent = Q_NULLPTR );

    // PROPERTIES
    static QString LATITUDE_PROP;
    static QString LONGITUDE_PROP;
    static QString ELEVATION_PROP;

    // GETTERS
    double getX() const;
    double getY() const;
    double getZ() const;
    double getLatitude() const;
    double getLongitude() const;
    double getAltitude() const;
    bool isNull() const;
    double distance( GWSCoordinate* other ) const;

    // OPERATORS
    bool operator == (const GWSCoordinate&) const;
    bool operator != (const GWSCoordinate&) const;
    GWSCoordinate& operator= (const GWSCoordinate&);

};

Q_DECLARE_METATYPE(GWSCoordinate*)

#endif // GWSCOORDINATE_H
