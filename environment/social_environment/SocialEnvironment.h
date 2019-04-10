#ifndef GWSSOCIALENVIRONMENT_H
#define GWSSOCIALENVIRONMENT_H

#include "../../environment/Environment.h"
#include "SocialRelation.h"

class GWSSocialEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSSocialEnvironment* globalInstance();

    // GETTERS
    QStringList getOutRelationeds( QSharedPointer<GWSAgent> agent , QString relation_type );
    QStringList getInRelationeds( QSharedPointer<GWSAgent> agent , QString relation_type );

    // METHODS
    //virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    //virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

private:

    GWSSocialEnvironment();
    GWSSocialEnvironment(GWSSocialEnvironment const&);
    ~GWSSocialEnvironment();

    QMap< QString , QMap < QString , QList< GWSSocialRelation > > > out_relations; // QMAP< Relation type , QMAP< Emitter , Relations > >
    QMap< QString , QMap < QString , QList< GWSSocialRelation > > > in_relations; // QMAP< Relation type , QMAP< Receiver , Relations > >

};

#endif // GWSSOCIALENVIRONMENT_H
