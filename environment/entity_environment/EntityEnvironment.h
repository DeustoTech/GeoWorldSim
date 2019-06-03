#ifndef GWSENTITYENVIRONMENT_H
#define GWSENTITYENVIRONMENT_H

#include <QSharedPointer>

#include "../../environment/Environment.h"
#include "../../util/storage/ObjectStorage.h"

class GWSEntityEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSEntityEnvironment* globalInstance();

    // GETTERS
    quint64 getAmount() const;
    bool contains( QString class_name ) const;

    QSharedPointer<GWSEntity> getRandomByClass( QString class_name );
    template <class T> QSharedPointer<T> getRandomByClass( QString class_name );

    QSharedPointer<GWSEntity> getByClassAndUID( QString class_name , QString internal_id ) const;  // Get one agent
    template <class T> QSharedPointer<T> getByClassAndUID( QString class_name , QString internal_id ) const;  // Get one agent

    QSharedPointer<GWSEntity> getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name

    QList< QSharedPointer<GWSEntity> > getByClass( QString class_name ) const; // Get all agents for a given type
    template <class T> QList<QSharedPointer<T>> getByClass( QString class_name ) const;  // Get all agents for a given type

    QSharedPointer<GWSEntity> getByUID( QString id ) const;
    QList< QSharedPointer<GWSEntity> > getByUIDS( QStringList ids ) const;
    QList< QSharedPointer<GWSEntity> > getByUIDS( QJsonArray ids ) const;

    QSharedPointer<GWSEntity> getByName(QString name) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByName(QString name) const; // Get one agent by its name

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> agent);
    virtual void unregisterEntity( QSharedPointer<GWSEntity> agent);

private:

    GWSEntityEnvironment();
    GWSEntityEnvironment(GWSEntityEnvironment const&);
    ~GWSEntityEnvironment();

    // ENVIRONMENT MEMORY
    GWSObjectStorage* environment_entities = Q_NULLPTR;

};

#endif // GWSENTITYENVIRONMENT_H
