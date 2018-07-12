#ifndef CLIMATEENVIRONMENT_H
#define CLIMATEENVIRONMENT_H

#include "../../environment/Environment.h"

#include "../../util/grid/Grid.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/units/Units.h"

class GWSClimateEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSEnvironment; // Environment can access

public:
    static GWSClimateEnvironment* globalInstance();

    // EXPORTERS
    //QJsonObject toJSON();
    //void fromJSON(QJsonObject json);

    // GETTERS
    GWSTemperatureUnit getTemperature( GWSCoordinate coor ) const;
    double getHumidity( GWSCoordinate coor ) const;
    double getSunIrradiation( GWSCoordinate coor ) const;

    // SETTERS
    void addTemperature( GWSCoordinate coor , GWSTemperatureUnit value );
    void addHumidity( GWSCoordinate coor , double value );
    void addSunIrradiation( GWSCoordinate coor , double value );

private:
    GWSClimateEnvironment();
    GWSClimateEnvironment(GWSClimateEnvironment const&);
    ~GWSClimateEnvironment();

    // Environment temperatures
    GWSGrid temperature;

    // Environment humidity
    GWSGrid humidity;

    // Environment sun irradiation
    GWSGrid sun_irradiation;

};

#endif // CLIMATEENVIRONMENT_H
