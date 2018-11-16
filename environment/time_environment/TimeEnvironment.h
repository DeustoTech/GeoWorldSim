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
    static QString WAIT_FOR_ME_PROP;

    // IMPORTERS
    void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize();

    // GETTERS
    qint64 getCurrentDateTime() const;
    double getTimeSpeed() const;
    qint64 getAgentInternalTime( QSharedPointer<GWSAgent> agent );

    // SETTERS
    void setDatetime( quint64 current_datetime_msecs );
    void setTimeSpeed(double time_speed = 1);
    void setAgentInternalTime( QSharedPointer<GWSAgent> agent , qint64 datetime );
    //void incrementAgentInternalTime( QSharedPointer<GWSAgent> agent , GWSTimeUnit seconds );

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:
    GWSTimeEnvironment();
    GWSTimeEnvironment(GWSTimeEnvironment const&);
    ~GWSTimeEnvironment();

    quint64 simulating_datetime_msecs;
    quint64 started_datetime_msecs;

    // Time speed
    // (How many seconds will there be in a real world second)
    double time_speed = 8;

    // Agent internal times
    QMap<QString , quint64> agent_internal_times;
};

#endif // GWSTIMEENVIRONMENT_H
