#ifndef GWSUNITS
#define GWSUNITS

#include <QVariant>
#include <QDebug>

/**********************************************************************
 BASE UNIT CLASS
**********************************************************************/

struct GWSUnit {
    double value = 0;
    const char* unit = "undefined";
    QString type = Q_NULLPTR;

    // CONSTRUCTORS
    GWSUnit() : value(0) , unit("undefined"){}
    GWSUnit(double value, const char* unit) : value(value) , unit( unit ){ Q_ASSERT(unit); } // ATTENTION, CAREFULL WITH IMPLICIT SHARING
    GWSUnit(const GWSUnit &other) : GWSUnit(other.value , other.unit ){}

    //GWSUnit() : type(0) , unit("undefined"){}
    GWSUnit(QString type, const char* unit) : type(type) , unit( unit ){ Q_ASSERT(unit); } // ATTENTION, CAREFULL WITH IMPLICIT SHARING
    //GWSUnit(const GWSUnit &other) : GWSUnit(other.type , other.unit ){}
    ~GWSUnit(){}

    // GETTERS
    double number() const { return value; }

    // SETTERS
    GWSUnit operator =(const double value){
        this->value = value;
        return *this;
    }

    GWSUnit operator =(const GWSUnit other){
        Q_ASSERT( this->unit == other.unit );
        this->value = other.value;
        return *this;
    }

    // COMPARE
    bool operator <(const GWSUnit other) const {
        Q_ASSERT( unit == other.unit );
        return value < other.value;
    }
    bool operator <=(const GWSUnit other) const{
        Q_ASSERT( unit == other.unit );
        return value <= other.value;
    }
    bool operator >(const GWSUnit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value > other.value;
    }
    bool operator >=(const GWSUnit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value >= other.value;
    }
    bool operator ==(const GWSUnit other) const{
        Q_ASSERT( unit == other.unit );
        return this->value == other.value;
    }
    bool operator ==(const double other) const{
        return this->value == other;
    }

    // MATH
    GWSUnit operator +(const GWSUnit other) const{
        Q_ASSERT( this->unit == other.unit );
        return GWSUnit( this->value + other.value , this->unit );
    }
    GWSUnit operator +(const double v) const{
        return GWSUnit( this->value + v , this->unit );
    }

    GWSUnit operator -(const GWSUnit other) const{
        Q_ASSERT( this->unit == other.unit );
        return GWSUnit( this->value - other.value , this->unit );
    }
    GWSUnit operator -(const double v) const{
        return GWSUnit( this->value - v , this->unit );
    }

    GWSUnit operator *(const double v) const{
        return GWSUnit( this->value * v , this->unit );
    }
    GWSUnit operator /(const double v) const{
        return GWSUnit( this->value / v , this->unit );
    }
};

/**********************************************************************
 TIME IN SECONDS
**********************************************************************/

struct GWSTimeUnit : GWSUnit {
    GWSTimeUnit(double seconds) : GWSUnit(seconds , "second"){}
    GWSTimeUnit(const GWSUnit &other) : GWSTimeUnit(other.value){}
    GWSTimeUnit() : GWSTimeUnit(0){}
};

/**********************************************************************
 LENGTH IN METERS
**********************************************************************/

struct GWSLengthUnit : GWSUnit {
    GWSLengthUnit(double meters) : GWSUnit(meters , "meter"){}
    GWSLengthUnit(const GWSUnit &other) : GWSLengthUnit(other.value){}
    GWSLengthUnit() : GWSLengthUnit(0){}
};

/**********************************************************************
 MASS IN GRAMS
**********************************************************************/

struct GWSMassUnit : GWSUnit {
    GWSMassUnit(double grams) : GWSUnit(grams , "gram"){}
    GWSMassUnit(const GWSUnit &other) : GWSMassUnit(other.value){}
    GWSMassUnit() : GWSMassUnit(0){}
};

/**********************************************************************
 PRICE IN EUROS
**********************************************************************/

struct GWSCurrencyUnit : GWSUnit {
    GWSCurrencyUnit(double euros) : GWSUnit(euros , "euro"){}
    GWSCurrencyUnit(const GWSUnit &other) : GWSCurrencyUnit(other.value){}
    GWSCurrencyUnit() : GWSCurrencyUnit(0){}
};

/**********************************************************************
 SPEED IN METERS PER SECOND
**********************************************************************/

struct GWSSpeedUnit : GWSUnit {
    GWSSpeedUnit(double meters_per_second) : GWSUnit(meters_per_second , "meter per second"){}
    GWSSpeedUnit(const GWSUnit &other) : GWSSpeedUnit(other.value){}
    GWSSpeedUnit() : GWSSpeedUnit(0){}
};

/**********************************************************************
 TEMPERATURE IN CELSIUS DEGREES
**********************************************************************/

struct GWSTemperatureUnit : GWSUnit {
    GWSTemperatureUnit(double celsius) : GWSUnit(celsius , "celsius degree"){}
    GWSTemperatureUnit(const GWSUnit &other) : GWSTemperatureUnit(other.value){}
    GWSTemperatureUnit() : GWSTemperatureUnit(0){}
};

/**********************************************************************
 AREA IN SQUARE METERS
**********************************************************************/

struct GWSAreaUnit : GWSUnit {
    GWSAreaUnit(double square_meters) : GWSUnit(square_meters , "square meter"){}
    GWSAreaUnit(const GWSUnit &other) : GWSAreaUnit(other.value){}
    GWSAreaUnit() : GWSAreaUnit(0){}
};

/**********************************************************************
 VOLUME UNITS IN LITER
**********************************************************************/

struct GWSVolumeUnit : GWSUnit {
    GWSVolumeUnit(double liters) : GWSUnit(liters , "liter"){}
    GWSVolumeUnit(const GWSUnit &other) : GWSVolumeUnit(other.value){}
    GWSVolumeUnit() : GWSVolumeUnit(0){}
};

/**********************************************************************
 POWER IN WATTS
**********************************************************************/

struct GWSPowerUnit : GWSUnit {
    GWSPowerUnit(double watts) : GWSUnit(watts , "watt"){}
    GWSPowerUnit(const GWSUnit &other) : GWSPowerUnit(other.value){}
    GWSPowerUnit() : GWSPowerUnit(0){}
};

/**********************************************************************
 FREQUENCY UNITS IN HERTZ
**********************************************************************/

struct GWSFrequencyUnit : GWSUnit {
    GWSFrequencyUnit(double hertzs) : GWSUnit(hertzs , "hertz"){}
    GWSFrequencyUnit(const GWSUnit &other) : GWSFrequencyUnit(other.value){}
    GWSFrequencyUnit() : GWSFrequencyUnit(0){}
};


/**********************************************************************
 RESOURCES TO BE USED AS INPUT/OUTPUT PROPERTIES
**********************************************************************/

struct GWSResourceUnit : GWSUnit {
    GWSResourceUnit(QString resource) : GWSUnit( resource, "resource"){}
    GWSResourceUnit(const GWSUnit &other) : GWSResourceUnit(other.type) {}
    GWSResourceUnit() : GWSResourceUnit(0){}
};

// DO NOT Declare Units to be used as QMETAPROPERTY
// NEED TO BE STORED AS DOUBLE, TO BE SERIALIZABLE

#endif // GWSUNITS

