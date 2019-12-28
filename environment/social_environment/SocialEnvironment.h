#ifndef SOCIALENVIRONMENT_H
#define SOCIALENVIRONMENT_H

#include "../../environment/Environment.h"
#include "SocialRelation.h"


namespace geoworldsim {
namespace environment {

class GWSSocialEnvironment : public Environment
{
    Q_OBJECT

public:
    static GWSSocialEnvironment* globalInstance();

    // GETTERS
    QStringList getOutRelationeds( QSharedPointer<geoworldsim::Entity> agent , QString relation_type );
    QStringList getInRelationeds( QSharedPointer<geoworldsim::Entity> agent , QString relation_type );

    // METHODS
    //virtual void registerAgent( QSharedPointer<GWSAgent> agent );
   // virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:

    GWSSocialEnvironment();
    GWSSocialEnvironment(GWSSocialEnvironment const&);
    ~GWSSocialEnvironment();

    QMap< QString , QMap < QString , QList< social::SocialRelation > > > out_relations; // QMAP< Relation type , QMAP< Emitter , Relations > >
    QMap< QString , QMap < QString , QList< social::SocialRelation > > > in_relations; // QMAP< Relation type , QMAP< Receiver , Relations > >

};

}
}

#endif // SOCIALENVIRONMENT_H
