#ifndef GWSAGENTENVIRONMENT_H
#define GWSAGENTENVIRONMENT_H

#include <QSharedPointer>

#include "../../environment/Environment.h"
#include "../../agent/Agent.h"
#include "../../util/storage/ObjectStorage.h"

class GWSAgentEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSAgentEnvironment* globalInstance();

    // GETTERS
    quint64 getAmount() const;
    QSharedPointer<GWSAgent> getRandomByClass( QString class_name );
    template <class T> QSharedPointer<T> getRandomByClass( QString class_name );

    QSharedPointer<GWSAgent> getByClassAndId( QString class_name , QString internal_id ) const;  // Get one agent
    template <class T> QSharedPointer<T> getByClassAndId( QString class_name , QString internal_id ) const;  // Get one agent

    QSharedPointer<GWSAgent> getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name

    QList< QSharedPointer<GWSAgent> > getByClass( QString class_name ) const; // Get all agents for a given type
    template <class T> QList<QSharedPointer<T>> getByClass( QString class_name ) const;  // Get all agents for a given type

    QSharedPointer<GWSAgent> getByName(QString name) const; // Get one agent by its name
    template <class T> QSharedPointer<T> getByName(QString name) const; // Get one agent by its name

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent);
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent);

private:

    GWSAgentEnvironment();
    GWSAgentEnvironment(GWSAgentEnvironment const&);
    ~GWSAgentEnvironment();

    // ENVIRONMENT MEMORY
    GWSObjectStorage* environment_agents;

};

#endif // GWSAGENTENVIRONMENT_H
