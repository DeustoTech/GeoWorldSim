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

    // PROPERTIES
    /**
    * @brief Agent's tick will be called when this datetime has been reached
    * To be compared with the TimeEnvironment and call this agent's tick
    * IMPORTANT Given in MSecsSinceEpoch
    */
    static QString INTERNAL_TIME_PROP;

    // IMPORTERS
    void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize();

    // GETTERS
    qint64 getCurrentDateTime() const;
    double getTimeSpeed() const;
    qint64 getAgentInternalTime( const GWSAgent* agent ) const;
    qint64 getAgentInternalTime( QString agent_id ) const;

    // SETTERS
    void setDatetime( quint64 current_datetime_msecs );
    void setTimeSpeed(double time_speed = 1);
    void setAgentInternalTime( const GWSAgent* agent , const qint64 datetime );
    void incrementAgentInternalTime( const GWSAgent* agent , GWSTimeUnit seconds );

    // REGISTRATION
    virtual void registerAgent( GWSAgent* agent , quint64 init_internal_time = 0 );
    virtual void unregisterAgent( GWSAgent* agent );

private:
    GWSTimeEnvironment();
    GWSTimeEnvironment(GWSTimeEnvironment const&);
    ~GWSTimeEnvironment();

    quint64 started_datetime_msecs;

    // Time speed
    // (How many seconds will there be in a real world second)
    double time_speed = 1;

    // Agent internal times
    QMap<QString , quint64> agent_internal_times;

};

#endif // GWSTIMEENVIRONMENT_H
