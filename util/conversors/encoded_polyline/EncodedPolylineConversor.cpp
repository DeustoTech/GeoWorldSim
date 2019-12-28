#include "EncodedPolylineConversor.h"

EncodedPolylineConversor::EncodedPolylineConversor(QObject *parent) : QObject(parent){

}

/**********************************************************************
 METHODS
**********************************************************************/

/** CALLER GETS OWNERSHIP OF POINTS LIST
 * @brief EncodedPolylineConversor::decodePolyline
 * @param factory
 * @param encoded
 * @return
 */
QList<GWSCoordinate> EncodedPolylineConversor::decodePolyline( QString encoded){
    QList<GWSCoordinate> coors;

    int index = 0;
    int lat = 0, lng = 0;

    while ( index < encoded.size() ) {
        int b, shift = 0, result = 0;

        do {
            b = encoded.at(index++).unicode() - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);

        int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
        lat += dlat;

        shift = 0;
        result = 0;

        do {
            b = encoded.at(index++).unicode() - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);

        int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
        lng += dlng;

        GWSCoordinate coor = GWSCoordinate( lng / PRECISION , lat / PRECISION );
        coors.append( coor );
    }

    return coors;
}
