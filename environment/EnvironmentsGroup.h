#ifndef ENVIRONMENTSGROUP_H
#define ENVIRONMENTSGROUP_H

#include "../../object/Object.h"
#include "../../environment/Environment.h"

namespace geoworldsim {
namespace environment {

class EnvironmentsGroup : public Object
{
    Q_OBJECT

public:
    static EnvironmentsGroup* globalInstance();

    // SETTERS
     void addEnvironment( Environment* subenvironment );

    void registerEntity( QSharedPointer< Entity > entity );
    void unregisterEntity( QSharedPointer< Entity > entity );

protected:
    EnvironmentsGroup();
    EnvironmentsGroup(EnvironmentsGroup const&);
    ~EnvironmentsGroup();

private:
     // To be notified when registering agents
     QList< Environment* > sub_environments;

};

}
}

#endif // ENVIRONMENTSGROUP_H
