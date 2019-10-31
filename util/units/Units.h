#ifndef UNITS
#define UNITS

#include <QVariant>
#include <QDebug>

/**********************************************************************
 BASE UNIT CLASS
**********************************************************************/

namespace geoworldsim {
namespace unit {


struct Unit {
    double value = 0;
    const char* unit = "undefined";
    QString type = Q_NULLPTR;

    // CONSTRUCTORS
    Unit() : value(0) , unit("undefined"){}
    Unit(double value, const char* unit) : value(value) , unit( unit ){ Q_ASSERT(unit); } // ATTENTION, CAREFULL WITH IMPLICIT SHARING
    Unit(const Unit &other) : Unit(other.value , other.unit ){}

    //GWSUnit() : type(0) , unit("undefined"){}
    Unit(QString type, const char* unit) : type(type) , unit( unit ){ Q_ASSERT(unit); } // ATTENTION, CAREFULL WITH IMPLICIT SHARING
    //GWSUnit(const GWSUnit &other) : GWSUnit(other.type , other.unit ){}
    ~Unit(){}

    // GETTERS
    double number() const { return value; }
    bool isValid() const { return this->number() == this->number();}

    // SETTERS
    Unit operator =(const double value){
        this->value = value;
        return *this;
    }

    Unit operator =(const Unit other){
        Q_ASSERT( this->unit == other.unit );
        this->value = other.value;
        return *this;
    }

    // COMPARE
    bool operator <(const Unit other) const {
        Q_ASSERT( unit == other.unit );
        return value < other.value;
    }
    bool operator <=(const Unit other) const{
        Q_ASSERT( unit == other.unit );
        return value <= other.value;
    }
    bool operator >(const Unit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value > other.value;
    }
    bool operator >=(const Unit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value >= other.value;
    }
    bool operator ==(const Unit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value == other.value;
    }
    bool operator ==(const double other) const{
        return this->value == other;
    }

    // MATH
    Unit operator +(const Unit other) const{
        Q_ASSERT( this->unit == other.unit );
        return Unit( this->value + other.value , this->unit );
    }
    Unit operator +(const double v) const{
        return Unit( this->value + v , this->unit );
    }
    Unit operator +=(const Unit other){
        Q_ASSERT( this->unit == other.unit );
        this->value += this->value + other.value;
        return Unit( this->value , this->unit );
    }
    Unit operator +=(const double v){
        this->value += v;
        return Unit( this->value , this->unit );
    }

    Unit operator -(const Unit other) const{
        Q_ASSERT( this->unit == other.unit );
        return Unit( this->value - other.value , this->unit );
    }
    Unit operator -(const double v) const{
        return Unit( this->value - v , this->unit );
    }

    Unit operator *(const Unit other) const{
        return Unit( this->value * other.value , "" );
    }
    Unit operator *(const double v) const{
        return Unit( this->value * v , this->unit );
    }
    Unit operator /(const Unit other) const{
        return Unit( this->value / other.value , "" );
    }
    Unit operator /(const double v) const{
        return Unit( this->value / v , this->unit );
    }
};

/**********************************************************************
 TIME IN SECONDS
**********************************************************************/

struct TimeUnit : Unit {
    TimeUnit(double seconds) : Unit(seconds , "second"){}
    TimeUnit(const Unit &other) : TimeUnit(other.value){}
    TimeUnit() : TimeUnit(0){}
};

/**********************************************************************
 LENGTH IN METERS
**********************************************************************/

struct LengthUnit : Unit {
    LengthUnit(double meters) : Unit(meters , "meter"){}
    LengthUnit(const Unit &other) : LengthUnit(other.value){}
    LengthUnit() : LengthUnit(0){}
};

/**********************************************************************
 MASS IN GRAMS
**********************************************************************/

struct MassUnit : Unit {
    MassUnit(double grams) : Unit(grams , "gram"){}
    MassUnit(const Unit &other) : MassUnit(other.value){}
    MassUnit() : MassUnit(0){}
};

/**********************************************************************
 PRICE IN EUROS
**********************************************************************/

struct CurrencyUnit : Unit {
    CurrencyUnit(double euros) : Unit(euros , "euro"){}
    CurrencyUnit(const Unit &other) : CurrencyUnit(other.value){}
    CurrencyUnit() : CurrencyUnit(0){}
};

/**********************************************************************
 SPEED IN METERS PER SECOND
**********************************************************************/

struct SpeedUnit : Unit {
    SpeedUnit(double meters_per_second) : Unit(meters_per_second , "meter per second"){}
    SpeedUnit(const Unit &other) : SpeedUnit(other.value){}
    SpeedUnit() : SpeedUnit(0){}
};

/**********************************************************************
 TEMPERATURE IN CELSIUS DEGREES
**********************************************************************/

struct TemperatureUnit : Unit {
    TemperatureUnit(double celsius) : Unit(celsius , "celsius degree"){}
    TemperatureUnit(const Unit &other) : TemperatureUnit(other.value){}
    TemperatureUnit() : TemperatureUnit(0){}
};

/**********************************************************************
 AREA IN SQUARE METERS
**********************************************************************/

struct AreaUnit : Unit {
    AreaUnit(double square_meters) : Unit(square_meters , "square meter"){}
    AreaUnit(const Unit &other) : AreaUnit(other.value){}
    AreaUnit() : AreaUnit(0){}
};

/**********************************************************************
 VOLUME UNITS IN LITER
**********************************************************************/

struct VolumeUnit : Unit {
    VolumeUnit(double liters) : Unit(liters , "liter"){}
    VolumeUnit(const Unit &other) : VolumeUnit(other.value){}
    VolumeUnit() : VolumeUnit(0){}
};

/**********************************************************************
 POWER IN WATTS
**********************************************************************/

struct PowerUnit : Unit {
    PowerUnit(double watts) : Unit(watts , "watt"){}
    PowerUnit(const Unit &other) : PowerUnit(other.value){}
    PowerUnit() : PowerUnit(0){}
};

/**********************************************************************
 FREQUENCY UNITS IN HERTZ
**********************************************************************/

struct FrequencyUnit : Unit {
    FrequencyUnit(double hertzs) : Unit(hertzs , "hertz"){}
    FrequencyUnit(const Unit &other) : FrequencyUnit(other.value){}
    FrequencyUnit() : FrequencyUnit(0){}
};


/**********************************************************************
 RESOURCES TO BE USED AS INPUT/OUTPUT PROPERTIES
**********************************************************************/

struct ResourceUnit : Unit {
    ResourceUnit(QString resource) : Unit( resource, "resource"){}
    ResourceUnit(const Unit &other) : ResourceUnit(other.type) {}
    ResourceUnit() : ResourceUnit(0){}
};


}
}

// DO NOT Declare Units to be used as QMETAPROPERTY
// NEED TO BE STORED AS DOUBLE, TO BE SERIALIZABLE

#endif // UNITS

