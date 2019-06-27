#ifndef GWSENVIRONMENT_H
#define GWSENVIRONMENT_H

#include "../../entity/Entity.h"

class GWSEnvironment : public GWSEntity
{
    Q_OBJECT

public:

    // PROPERTIES
    static QString SKIP_INDEXING;
    static QString SKIP_SUBSCRIBING;

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> entity );
    virtual void unregisterEntity( QSharedPointer<GWSEntity> entity );

protected:
    GWSEnvironment();
    GWSEnvironment(GWSEnvironment const&);
    ~GWSEnvironment();

};

#endif // GWSENVIRONMENT_H
