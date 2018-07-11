#ifndef TILECOORDINATESCONVERSOR_H
#define TILECOORDINATESCONVERSOR_H

#include <QObject>

class TileCoordinatesConversor : public QObject
{
    Q_OBJECT

public:
    explicit TileCoordinatesConversor(QObject *parent = 0);

    // GETTERS
    static double tilex2lon( int x , int z ); // Given tile X get left-top longitude
    static double tiley2lat( int y , int z ); // Given tile Y get left-top latitude

    static int long2tilex(double lon, int z); // Given a longitude, get the X tile
    static int lat2tiley(double lat, int z); // Given a latitude, get the Y tile
};

#endif // TILECOORDINATESCONVERSOR_H
