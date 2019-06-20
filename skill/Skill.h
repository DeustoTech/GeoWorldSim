#ifndef GWSSKILL_H
#define GWSSKILL_H

#include <QSharedPointer>

#include "../../object/Object.h"
#include "../../entity/Entity.h"

class GWSSkill : public GWSObject
{
    Q_OBJECT
    friend class GWSEntity; // Entity can check skill insides

public:
    Q_INVOKABLE explicit GWSSkill();

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json, QSharedPointer<GWSObject> skilled_entity);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer<GWSEntity> getEntity() const;
    virtual QJsonValue getProperty( QString name ) const;

private:
    QSharedPointer<GWSEntity> skilled_entity;
};

#endif // GWSSKILL_H
