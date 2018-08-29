#ifndef GWSENVIRONMENT_H
#define GWSENVIRONMENT_H

#include "../../agent/Agent.h"

class GWSEnvironment : public GWSAgent
{
    Q_OBJECT

public:
    static GWSEnvironment* globalInstance();

    void registerSubenvironment( GWSEnvironment* subenvironment );
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

signals:
    void environmentCreatedSignal();
    void environmentDeletedSignal();

protected:
    GWSEnvironment();
    GWSEnvironment(GWSEnvironment const&);
    ~GWSEnvironment();

private:
    // To be notified when registering agents
    QList<GWSEnvironment*> sub_environments;

};

#endif // GWSENVIRONMENT_H
