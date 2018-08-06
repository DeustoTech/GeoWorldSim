#ifndef GWSAGENTENVIRONMENT_H
#define GWSAGENTENVIRONMENT_H

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
    GWSAgent* getRandomByClass( QString class_name );
    template <class T> T* getRandomByClass( QString class_name );

    GWSAgent* getByClassAndId( QString class_name , QString internal_id ) const;  // Get one agent
    template <class T> T* getByClassAndId( QString class_name , QString internal_id ) const;  // Get one agent

    GWSAgent* getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name
    template <class T> T* getByClassAndName( QString class_name , QString name ) const; // Get one agent by its name

    QList< GWSAgent* > getByClass( QString class_name ) const; // Get all agents for a given type
    template <class T> QList<T*> getByClass( QString class_name ) const;  // Get all agents for a given type

    GWSAgent* getByName(QString name) const; // Get one agent by its name
    template <class T> T* getByName(QString name) const; // Get one agent by its name


protected:
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:

    GWSAgentEnvironment();
    GWSAgentEnvironment(GWSAgentEnvironment const&);
    ~GWSAgentEnvironment();

    // ENVIRONMENT MEMORY
    GWSObjectStorage* environment_agents;

};

#endif // GWSAGENTENVIRONMENT_H
