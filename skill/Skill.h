#ifndef SKILL_H
#define SKILL_H

#include <QSharedPointer>

#include "../../object/Object.h"
#include "../../entity/Entity.h"

namespace geoworldsim {
namespace skill {


class Skill : public Object
{
    Q_OBJECT
    friend class geoworldsim::Entity; // Entity can check skill insides

public:
    Q_INVOKABLE explicit Skill();

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json, QSharedPointer<Object> skilled_entity);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer< Entity > getEntity() const;
    virtual QJsonValue getProperty( const QString& name ) const;

private:
    QSharedPointer< Entity > skilled_entity;
};


}
}

Q_DECLARE_METATYPE( QSharedPointer<geoworldsim::skill::Skill> ) // REQUIRED TO BE USED IN SIGNAL AND SLOTS

#endif // SKILL_H
