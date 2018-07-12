#ifndef GWSENVIRONMENT_H
#define GWSENVIRONMENT_H

#include "../../agent/Agent.h"

class GWSEnvironment : public GWSAgent
{
    Q_OBJECT

public:
    static GWSEnvironment* globalInstance();

    // METHODS
    virtual void registerAgent( GWSAgent* agent );
    virtual void unregisterAgent( GWSAgent* agent );
    void registerSubenvironment( GWSEnvironment* subenvironment );

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
