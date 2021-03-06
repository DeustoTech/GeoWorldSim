#ifndef GWSTIMEENVIRONMENT_H
#define GWSTIMEENVIRONMENT_H

#include <QDateTime>
#include <QJsonObject>

#include "../../environment/Environment.h"
#include "../../util/units/Units.h"

namespace geoworldsim {
namespace environment {


class TimeEnvironment : public Environment
{
    Q_OBJECT

public:
    static TimeEnvironment* globalInstance();

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
    //qint64 getAgentInternalTime( QSharedPointer<GWSAgent> agent );

    // SETTERS
    void setDatetime( quint64 datetime );
    void goBackToDatetime( quint64 datetime ); // Sets the min between current or the given
    void goBackMsecs( quint64 msecs ); // Decrement current
    void setTimeSpeed( double time_speed = 1 );
    //void setAgentInternalTime( QSharedPointer<GWSAgent> agent , qint64 datetime );
    //void incrementAgentInternalTime( QSharedPointer<GWSAgent> agent , GWSTimeUnit seconds );

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> agent );
    virtual void unregisterEntity( QSharedPointer<Entity> agent );

protected slots:
    void entityPropertyChanged( QString property_name );

private:
    TimeEnvironment();
    TimeEnvironment(TimeEnvironment const&);
    ~TimeEnvironment();

    quint64 simulation_datetime_msecs;
    quint64 software_started_datetime_msecs;

    // Time speed
    // (How many seconds will there be in a real world second)
    double time_speed = 1;

};

}
}

#endif // GWSTIMEENVIRONMENT_H
