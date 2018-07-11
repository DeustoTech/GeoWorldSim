#ifndef PROJECTIONCONVERSOR_H
#define PROJECTIONCONVERSOR_H

#include <QObject>
#include <QDebug>
#include "util/geometry/GeometryFactory.h"

#include "proj_api.h"

class ProjectionConversor : public QObject
{
    Q_OBJECT
public:
    static ProjectionConversor* getInstance();

    // METHODS
    GWSCoordinate convert( double x , double y , double z , int from_epsg_code , int to_epsg_code );
    GWSCoordinate convert( GWSCoordinate coor , int from_epsg_code , int to_epsg_code);
    double getDistanceInMeters(double lon1, double lat1, double lon2, double lat2);

private:
    ProjectionConversor();
    ProjectionConversor(ProjectionConversor const&);
    ~ProjectionConversor();

    QMap< int , const char * > proj_commands;
};

#endif // PROJECTIONCONVERSOR_H
