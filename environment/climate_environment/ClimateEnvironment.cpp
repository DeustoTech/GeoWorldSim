#include "ClimateEnvironment.h"

#include "util/parallelism/ParallelismController.h"
#include "util/geometry/Envelope.h"

GWSClimateEnvironment* GWSClimateEnvironment::globalInstance(){
    static GWSClimateEnvironment instance;
    return &instance;
}

GWSClimateEnvironment::GWSClimateEnvironment() : GWSEnvironment() {
    qInfo() << "ClimateEnvironment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSClimateEnvironment::~GWSClimateEnvironment(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSTemperatureUnit GWSClimateEnvironment::getTemperature( GWSCoordinate coor ) const{
    return GWSTemperatureUnit( this->temperature.getValue( coor ) );
}

double GWSClimateEnvironment::getHumidity( GWSCoordinate coor ) const{
    return this->humidity.getValue( coor );
}

double GWSClimateEnvironment::getSunIrradiation( GWSCoordinate coor ) const{
    return this->sun_irradiation.getValue( coor );
}


/**********************************************************************
 SETTERS
**********************************************************************/

void GWSClimateEnvironment::addTemperature(GWSCoordinate coor, GWSTemperatureUnit value){
    this->temperature.setValue( coor , value.number() );
}

void GWSClimateEnvironment::addHumidity(GWSCoordinate coor, double value){
    this->humidity.setValue( coor , value );
}

void GWSClimateEnvironment::addSunIrradiation(GWSCoordinate coor, double value){
    this->sun_irradiation.setValue( coor , value );
}
