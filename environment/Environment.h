#ifndef GWSENVIRONMENT_H
#define GWSENVIRONMENT_H

#include "../../agent/Agent.h"

class GWSEnvironment : public GWSAgent
{
    Q_OBJECT

public:

    // PROPERTIES
    static QString SKIP_INDEXING;

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

signals:
    void environmentCreatedSignal();
    void environmentDeletedSignal();

protected:
    GWSEnvironment();
    GWSEnvironment(GWSEnvironment const&);
    ~GWSEnvironment();

};

#endif // GWSENVIRONMENT_H
