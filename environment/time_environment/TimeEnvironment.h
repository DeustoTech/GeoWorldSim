#ifndef GWSTIMEENVIRONMENT_H
#define GWSTIMEENVIRONMENT_H

#include <QDateTime>
#include <QJsonObject>

#include "../../environment/Environment.h"
#include "../../util/units/Units.h"
/**
 * @brief The TimeEnvironment class
 * Class to be used in environment to get time / hours.
 */
class GWSTimeEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSTimeEnvironment* globalInstance();

    // IMPORTERS
    void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize();

    // GETTERS
    qint64 getCurrentDateTime() const;
    double getTimeSpeed() const;

    // SETTERS
    void setDatetime( quint64 current_datetime_msecs );
    void setTimeSpeed(double time_speed = 1);

    // METHODS
    virtual void registerAgent( GWSAgent* agent );
    virtual void unregisterAgent( GWSAgent* agent );

private:
    GWSTimeEnvironment();
    GWSTimeEnvironment(GWSTimeEnvironment const&);
    ~GWSTimeEnvironment();

    quint64 started_datetime_msecs;

    // Time speed
    // (How many seconds will there be in a real world second)
    double time_speed = 2;

};

#endif // GWSTIMEENVIRONMENT_H
