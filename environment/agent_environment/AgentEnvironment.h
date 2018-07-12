#ifndef GWSAGENTENVIRONMENT_H
#define GWSAGENTENVIRONMENT_H

#include "../../environment/Environment.h"
#include "../../agent/Agent.h"
#include "../../util/storage/ObjectStorage.h"

class GWSAgentEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSEnvironment; // Environment can access

public:
    static GWSAgentEnvironment* globalInstance();

    // EXPORTERS
    virtual QJsonObject serialize();
    virtual QImage toImage(unsigned int image_width = 1024 , unsigned int image_height = 1024 , QStringList class_names = QStringList( GWSAgent::staticMetaObject.className() ) );

    // IMPORTERS
    void deserialize(QJsonObject json);

    // GETTERS
    quint64 getAmount() const;
    GWSAgent* getByClassAndId( const QMetaObject metaobject , QString internal_id ) const;  // Get one agent
    template <class T> T* getByClassAndId( const QMetaObject metaobject , QString internal_id ) const;  // Get one agent

    GWSAgent* getByClassAndName( const QMetaObject metaobject , QString name ) const; // Get one agent by its name
    template <class T> T* getByClassAndName( const QMetaObject metaobject , QString name ) const; // Get one agent by its name

    QList< GWSAgent* > getByClass( const QMetaObject metaobject ) const; // Get all agents for a given type
    template <class T> QList<T*> getByClass( const QMetaObject metaobject ) const;  // Get all agents for a given type

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
