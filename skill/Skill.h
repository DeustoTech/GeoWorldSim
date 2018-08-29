#ifndef GWSSKILL_H
#define GWSSKILL_H

#include <QSharedPointer>

#include "../../object/Object.h"
#include "../../agent/Agent.h"

class GWSSkill : public GWSObject
{
    Q_OBJECT
    friend class GWSAgent; // Agent can check skill insides

public:
    Q_INVOKABLE explicit GWSSkill();

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer<GWSAgent> getAgent();
};

#endif // GWSSKILL_H
