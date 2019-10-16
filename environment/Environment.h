#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../entity/Entity.h"

namespace geoworldsim {
namespace environment {


class Environment : public geoworldsim::Entity
{
    Q_OBJECT

public:

    // PROPERTIES
    static QString SKIP_INDEXING;
    static QString SKIP_SUBSCRIBING;

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json , QSharedPointer<Object> parent = QSharedPointer<Object>() );

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> entity );
    virtual void unregisterEntity( QSharedPointer<Entity> entity );

protected:
    Environment();
    Environment(Environment const&);
    ~Environment();

};

}
}

#endif // ENVIRONMENT_H
