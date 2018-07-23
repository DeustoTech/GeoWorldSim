#ifndef GWSCOORDINATE_H
#define GWSCOORDINATE_H

#include <QPointF>
#include <QString>

/**
 * @brief The GeoCoordinates class
 * @url https://schema.org/GeoCoordinates
 */
class GWSCoordinate
{
    friend class GWSGeometryController;
    friend class GWSGraphNode;
    friend class GWSGraphEdge;

public:
    explicit GWSCoordinate();

    // PROPERTIES
    static QString LATITUDE_PROP;
    static QString LONGITUDE_PROP;
    static QString ELEVATION_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    double getX() const;
    double getY() const;
    double getZ() const;
    double getLatitude() const;
    double getLongitude() const;
    double getElevation() const;
    bool isNull() const;
    double distance( GWSCoordinate* other ) const;

    // OPERATORS
    bool operator == (const GWSCoordinate&) const;
    bool operator != (const GWSCoordinate&) const;
    GWSCoordinate& operator= (const GWSCoordinate&);

private:
    double latitude;
    double longitude;
    double altitude;

};

Q_DECLARE_METATYPE(GWSCoordinate)

#endif // GWSCOORDINATE_H
