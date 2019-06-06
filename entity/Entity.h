#ifndef GWSENTITY_H
#define GWSENTITY_H

//#include <QtQml/QQmlEngine>
#include <QMetaType>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlRecord>
#include <QStringList>
#include <QMutex>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "./../object/Object.h"

#include "./../util/units/Units.h"
#include "./../util/storage/ObjectStorage.h"

QT_FORWARD_DECLARE_CLASS(GWSEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSPhysicalEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSExecutionEnvironment)
QT_FORWARD_DECLARE_CLASS(GWSSkill)
QT_FORWARD_DECLARE_CLASS(GWSBehaviour)

// Declare Coordinate to be used as QMETAPROPERTY
// Q_DECLARE_METATYPE(GWSGeometry)

using namespace QtConcurrent;

class GWSEntity : public GWSObject
{
    friend class GWSEnvironment; // Environment will be able to overwrite the attributes
    friend class GWSExecutionEnvironment;

    Q_OBJECT // Required for every child!! Adds Qt extra functionality like SIGNAL/SLOTS

public:

    Q_INVOKABLE explicit GWSEntity(); // Required Q_INVOKABLE to be registerd in GWSObjectFactory
    ~GWSEntity();

    // STYLE PROPERTIES
    static QString STYLE_COLOR_PROP;
    static QString STYLE_OPACITY_PROP;
    static QString STYLE_BORDER_COLOR_PROP;
    static QString STYLE_BORDER_WEIGHT_PROP;
    static QString STYLE_DASH_ARRAY_PROP;
    static QString STYLE_ICON_URL_PROP;
    static QString STYLE_ZOOM_LEVEL_PROP;

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // EXPORTERS
    virtual QJsonObject serialize() const; // Called when asked for more info about this entity. Can be overwritten if want to add extra variables to the exporters
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 );

    // GETTERS
    QList<GWSEnvironment*> getEnvironments() const;
    bool isBusy() const;
    bool fulfillsFilter( QJsonObject filter , bool nulls_allowed = true ) const;
    QSharedPointer<GWSEntity> getSharedPointer() const;

    // SKILLS
    bool hasSkill( QString class_name ) const;
    QSharedPointer<GWSSkill> getSkill( QString class_name , bool silent = false ) const;
    //template <class T> QSharedPointer<T> getSkill( QString class_name ) const;
    QList< QSharedPointer<GWSSkill> > getSkills( QString class_name ) const;
    //template <class T> QList< QSharedPointer<T> > getSkills( QString class_name ) const;

    // BEHAVIOURS
    QList< QSharedPointer<GWSBehaviour> > getCurrentlyExecutingBehaviours() const;
    QSharedPointer<GWSBehaviour> getBehaviour( QString behaviour_id ) const;
    QList< QSharedPointer<GWSBehaviour> > getBehaviours( QString class_name ) const;

    // SETTERS
    void incrementBusy();
    void decrementBusy();
    void addSkill( QSharedPointer<GWSSkill> skill );
    void removeSkill( QSharedPointer<GWSSkill> skill );
    void addBehaviour( QSharedPointer<GWSBehaviour> behaviour );
    void addCurrentlyExecutingBehaviour( QSharedPointer<GWSBehaviour> behaviour );

public slots:
    void run();
    void stop();

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the entity's thread
    virtual void tick() final; // Acts as a behave() wrapper

protected slots:
    virtual void behave(); // Behaviour, To be implemented by children, must be synchronous because tick is already invoked asyncrhonously

signals:
    void entityStartedSignal();
    void entityBehavedSignal();
    void entityEndedSignal();

protected:

    QList<GWSEnvironment*> environments_registerd_in; // List of environments it lives in

    /**
     * @brief Entity skills storage
     */
    GWSObjectStorage* skills = Q_NULLPTR;

    /**
     * @brief Entity behaviour
     */
    GWSObjectStorage* behaviours = Q_NULLPTR;
    QList< QSharedPointer<GWSBehaviour> > to_be_executed_behaviours;

    /**
      * Mutex for paralelism
      */
    mutable QMutex mutex;

private:

    int busy_counter = 0;
};

Q_DECLARE_METATYPE(GWSEntity*) // REQUIRED IN EVERY CHILD

#endif // GWSENTITY_H