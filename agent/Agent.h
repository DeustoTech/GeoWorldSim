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
#include "./../../util/geometry/Geometry.h"
//#include "utils/geometry/Coordinate.h"

//#include "utils/conversors/custom_json/CustomJsonConversor.h"

#include "./../util/units/Units.h"
#include "./../util/geometry/Geometry.h"
#include "./../util/uistyle/Style.h"
#include "./../util/storage/ObjectStorage.h"
#include "./../util/uistyle/Style.h"

QT_FORWARD_DECLARE_CLASS(GWSEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSPhysicalEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSExecutionEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSSkill)
QT_FORWARD_DECLARE_CLASS(GWSBehaviour)

// Declare Coordinate to be used as QMETAPROPERTY
// Q_DECLARE_METATYPE(GWSGeometry)

class GWSAgent : public GWSObject , public GWSStyle
{
    friend class GWSEnvironment; // Environment will be able to overwrite the attributes
    friend class GWSExecutionEnvironment;

    Q_OBJECT // Required for every child!! Adds Qt extra functionality like SIGNAL/SLOTS

public:

    Q_INVOKABLE explicit GWSAgent(); // Required Q_INVOKABLE to be registerd in GWSObjectFactory
    ~GWSAgent();

    // PROPERTIES
    static QString ALIVE_PROP;
    static QString STYLE_PROP;

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // EXPORTERS
    virtual QJsonObject serialize() const; // Called when asked for more info about this agent. Can be overwritten if want to add extra variables to the exporters
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 );

    // GETTERS
    QList<GWSEnvironment*> getEnvironments() const;
    bool isBusy() const;
    QSharedPointer<GWSAgent> getSharedPointer() const;

    // SKILLS
    bool hasSkill( QString class_name ) const;
    QSharedPointer<GWSSkill> getSkill( QString class_name ) const;
    //template <class T> QSharedPointer<T> getSkill( QString class_name ) const;
    QList< QSharedPointer<GWSSkill> > getSkills( QString class_name ) const;
    //template <class T> QList< QSharedPointer<T> > getSkills( QString class_name ) const;

    // BEHAVIOURS
    QList< QSharedPointer<GWSBehaviour> > getStartBehaviour() const;
    QSharedPointer<GWSBehaviour> getBehaviour( QString behaviour_id ) const;
    QList< QSharedPointer<GWSBehaviour> > getBehaviours( QString class_name ) const;

    // SETTERS
    void incrementBusy();
    void decrementBusy();
    void addSkill( QSharedPointer<GWSSkill> skill );
    void removeSkill( QSharedPointer<GWSSkill> skill );
    void addBehaviour( QSharedPointer<GWSBehaviour> behaviour );
    void addStartBehaviour( QSharedPointer<GWSBehaviour> behaviour );

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    virtual void tick() final; // Acts as a behave() wrapper
protected slots:
    virtual void behave(); // Behaviour, To be implemented by children, must be synchronous because tick is already invoked asyncrhonously

signals:
    void agentStartedSignal();
    void agentBehavedSignal();
    void agentEndedSignal();

protected:

    QList<GWSEnvironment*> environments_registerd_in; // List of environments it lives in

    /**
     * @brief Agent skills storage
     */
    GWSObjectStorage* skills = Q_NULLPTR;

    /**
     * @brief Agent behaviour
     */
    GWSObjectStorage* behaviours = Q_NULLPTR;
    QList< QSharedPointer<GWSBehaviour> > start_behaviours;

    /**
      * Mutex for paralelism
      */
    mutable QMutex mutex;

private:

    /**
     * @brief timer
     * Each running agent MUST have its timer, to schedule its slots.
     * It is not good to singleshot many slots in one timer, so each agent will have its own
     */
    QTimer* timer = Q_NULLPTR;

    int busy_counter = 0;
};

Q_DECLARE_METATYPE(GWSAgent*) // REQUIRED IN EVERY CHILD

#endif // GWSAGENT_H
