#ifndef GWSAGENT_H
#define GWSAGENT_H

//#include <QtQml/QQmlEngine>
#include <QMetaType>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlRecord>
#include <QStringList>
#include <QPointer>
#include <QImage>
#include <QMutex>
#include <QTimer>
#include <QDebug>

#include "./../object/Object.h"
//#include "utils/geometry/Coordinate.h"
//#include "utils/geometry/Geometry.h"
//#include "utils/conversors/custom_json/CustomJsonConversor.h"

#include "./../util/units/Units.h"
#include "./../util/storage/ObjectStorage.h"
#include "./../util/uistyle/UiStyle.h"

QT_FORWARD_DECLARE_CLASS(GWSEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSPhysicalEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSExecutionEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSSkill)
QT_FORWARD_DECLARE_CLASS(GWSBehaviour)

// Declare Coordinate to be used as QMETAPROPERTY
//Q_DECLARE_METATYPE(GWSGeometry)

class GWSAgent : public GWSObject
{
    friend class GWSEnvironment; // Environment will be able to overwrite the attributes
    friend class GWSPhysicalEnvironment;
    friend class GWSExecutionEnvironment;

    Q_OBJECT // Required for every child!! Adds Qt extra functionality like SIGNAL/SLOTS

public:

    Q_INVOKABLE explicit GWSAgent( QObject* parent = Q_NULLPTR ); // Required Q_INVOKABLE to be registerd in GWSObjectFactory
    ~GWSAgent();

    // PROPERTIES
    static QString RUNNING_PROP;
    static QString GEOMETRY_PROP;
    static QString STYLE_PROP;
        /**
        * @brief next_operation_datetime Agent's tick will be called when this datetime has been reached
        * To be compared with the TimeEnvironment and call this agent's tick
        * IMPORTANT Given in MSecsSinceEpoch
        */
    static QString INTERNAL_TIME_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const; // Called when asked for more info about this agent. Can be overwritten if want to add extra variables to the exporters
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 );

    // GETTERS
    //const GWSGeometry* getGeometry() const;
    //GWSCoordinate getRepresentativeCoordinate() const;
    bool isRunning() const;
    bool isBusy() const;
    qint64 getInternalTime() const;

    // SKILLS
    bool hasSkill( QString class_name ) const;
    GWSSkill* getSkill( QString class_name ) const;
    template <class T> T* getSkill( QString class_name ) const;
    QList<GWSSkill*> getSkills( QString class_name ) const;
    template <class T> QList<T*> getSkills( QString class_name ) const;

    // SETTERS
    //virtual void setGeometry( GSSGeometry* geometry );
    void setStartBehaviour( GWSBehaviour* start_behaviour );
    void setInternalTime( qint64 datetime );
    void incrementInternalTime( GWSTimeUnit seconds = GWSTimeUnit(0) );
    void incrementBusy();
    void decrementBusy();
    void addSkill( GWSSkill* skill );
    void removeSkill( GWSSkill* skill );

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    void tick(); // Acts as a behave() wrapper
    virtual void behave(); // Behaviour, To be implemented by children, must be synchronous because tick is already asyncrhonous

signals:
    void agentStartedSignal();
    void agentBehavedSignal();
    void agentEndedSignal();

protected:

    /**
     * @brief Agent skills storage
     */
    GWSObjectStorage* skills = Q_NULLPTR;

    /**
     * @brief Agent behaviour
     */
    GWSBehaviour* start_behaviour = Q_NULLPTR;

    /**
      * Mutex for paralelism
      */
    mutable QMutex mutex;

private:

     /**
     * @brief Agent's geometry
     */
    //GWSGeometry* geometry = Q_NULLPTR; // Agents geometry

    /**
     * @brief timer
     * Each running agent MUST have its timer, to schedule its slots.
     * It is not good to singleshot many slots in one timer, so each agent will have its own
     */
    QTimer* timer = Q_NULLPTR;

    int busy_counter = 0;
};

#endif // GWSAGENT_H
