#ifndef ENTITY_H
#define ENTITY_H

//#include <QtQml/QQmlEngine>
#include <QMetaType>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlRecord>
#include <QStringList>
#include <QDebug>
#include <QFuture>

#include "./../object/Object.h"
#include "./../util/storage/ObjectStorage.h"

namespace geoworldsim {

namespace environment {
    QT_FORWARD_DECLARE_CLASS(Environment)
    QT_FORWARD_DECLARE_CLASS(ExecutionEnvironment)
}
namespace skill {
    QT_FORWARD_DECLARE_CLASS(Skill)
}
namespace behaviour {
    QT_FORWARD_DECLARE_CLASS(Behaviour)
}

class Entity : public Object
{
    friend class environment::Environment; // Environment will be able to overwrite the attributes
    friend class environment::ExecutionEnvironment;

    Q_OBJECT // Required for every child!! Adds Qt extra functionality like SIGNAL/SLOTS

public:

    Q_INVOKABLE explicit Entity(); // Required Q_INVOKABLE to be registerd in GWSObjectFactory
    ~Entity();

    // STYLE PROPERTIES
    static QString STYLE_COLOR_PROP;
    static QString STYLE_OPACITY_PROP;
    static QString STYLE_BORDER_COLOR_PROP;
    static QString STYLE_BORDER_WEIGHT_PROP;
    static QString STYLE_DASH_ARRAY_PROP;
    static QString STYLE_ICON_URL_PROP;
    static QString STYLE_ZOOM_LEVEL_PROP;

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json , QSharedPointer<Object> parent = QSharedPointer<Object>() );

    // EXPORTERS
    virtual QJsonObject serialize() const; // Called when asked for more info about this entity. Can be overwritten if want to add extra variables to the exporters
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 );

    // GETTERS
    QList< environment::Environment* > getEnvironments() const;
    bool isBusy() const;
    bool fulfillsFilter( QJsonObject filter , bool nulls_allowed = true ) const;
    QSharedPointer<Entity> getSharedPointer() const;

    // SKILLS
    bool hasSkill( QString class_name ) const;
    QSharedPointer< skill::Skill > getSkill( QString class_name , bool silent = false ) const;
    QList< QSharedPointer< skill::Skill > > getSkills( QString class_name ) const;

    // BEHAVIOURS
    QList< QSharedPointer< behaviour::Behaviour > > getCurrentlyExecutingBehaviours() const;
    QStringList getCurrentlyExecutingBehaviourUIDS() const;
    QSharedPointer< behaviour::Behaviour > getBehaviour( QString behaviour_id ) const;
    QList< QSharedPointer< behaviour::Behaviour > > getBehaviours( QString class_name ) const;

    // SETTERS
    void incrementBusy();
    void decrementBusy();
    void addSkill( QSharedPointer< skill::Skill > skill );
    void removeSkill( QSharedPointer< skill::Skill > skill );
    void addBehaviour( QSharedPointer< behaviour::Behaviour > behaviour );
    void addCurrentlyExecutingBehaviour( QSharedPointer< behaviour::Behaviour > behaviour );

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

    QList< environment::Environment* > environments_registerd_in; // List of environments it lives in

    /**
     * @brief Entity skills storage
     */
    storage::ObjectStorage* skills = Q_NULLPTR;

    /**
     * @brief Entity behaviour
     */
    storage::ObjectStorage* behaviours = Q_NULLPTR;
    QList< QSharedPointer< behaviour::Behaviour > > to_be_executed_behaviours;

private:

    int busy_counter = 0;
};

}

Q_DECLARE_METATYPE( geoworldsim::Entity* ) // REQUIRED IN EVERY CHILD

#endif // GWSENTITY_H
