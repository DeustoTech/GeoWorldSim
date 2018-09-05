#ifndef GWSENVIRONMENTSGROUP_H
#define GWSENVIRONMENTSGROUP_H

#include "../../object/Object.h"
#include "../../environment/Environment.h"

class GWSEnvironmentsGroup : public GWSObject
{
    Q_OBJECT

public:
    static GWSEnvironmentsGroup* globalInstance();

    // SETTERS
     void addEnvironment( GWSEnvironment* subenvironment );

    void registerAgent( QSharedPointer<GWSAgent> agent );
    void unregisterAgent( QSharedPointer<GWSAgent> agent );

protected:
    GWSEnvironmentsGroup();
    GWSEnvironmentsGroup(GWSEnvironmentsGroup const&);
    ~GWSEnvironmentsGroup();

private:
     // To be notified when registering agents
     QList< GWSEnvironment* > sub_environments;

};

#endif // GWSENVIRONMENTSGROUP_H
