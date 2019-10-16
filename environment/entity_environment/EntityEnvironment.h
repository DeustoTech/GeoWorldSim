#ifndef ENTITYENVIRONMENT_H
#define ENTITYENVIRONMENT_H

#include <QSharedPointer>

#include "../../environment/Environment.h"
#include "../../util/storage/ObjectStorage.h"

namespace geoworldsim {
namespace environment {


class EntityEnvironment : public Environment
{
    Q_OBJECT

public:
    static EntityEnvironment* globalInstance();

    // GETTERS
    quint64 getAmount() const;
    bool contains( QString class_name ) const;

    QSharedPointer<Entity> getRandomByClass( QString class_name );
    template <class T> QSharedPointer<T> getRandomByClass( QString class_name ){
        return this->getRandomByClass( class_name ).dynamicCast<T>();
    }

    QSharedPointer<Entity> getByClassAndUID( QString class_name , QString internal_id ) const;  // Get one agent
    template <class T> QSharedPointer<T> getByClassAndUID( QString class_name , QString uid ) const{
        return this->environment_entities->getByClassAndUID<T>(class_name , uid);
    }

    QSharedPointer<Entity> getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByClassAndName( QString class_name , QString name ) const{
        return this->environment_entities->getByClassAndName<T>( class_name , name );
    }

    QList< QSharedPointer<Entity> > getByClass( QString class_name ) const; // Get all agents for a given type
    template <class T> QList<QSharedPointer<T>> getByClass( QString class_name ) const{
        return this->environment_entities->getByClass<T>( class_name );
    }

    QSharedPointer<Entity> getByUID( QString id ) const;
    QList< QSharedPointer<Entity> > getByUIDS( QStringList ids ) const;
    QList< QSharedPointer<Entity> > getByUIDS( QJsonArray ids ) const;

    QSharedPointer<Entity> getByName(QString name) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByName(QString name) const{
        return this->environment_entities->getByName<T>( name );
    }

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> agent);
    virtual void unregisterEntity( QSharedPointer<Entity> agent);

private:

    EntityEnvironment();
    EntityEnvironment(EntityEnvironment const&);
    ~EntityEnvironment();

    // ENVIRONMENT MEMORY
    storage::ObjectStorage* environment_entities = Q_NULLPTR;

};

}
}

#endif // GWSENTITYENVIRONMENT_H
