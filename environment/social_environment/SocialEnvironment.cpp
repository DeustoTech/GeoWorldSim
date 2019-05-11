#include "SocialEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

GWSSocialEnvironment* GWSSocialEnvironment::globalInstance(){
    static GWSSocialEnvironment instance;
    return &instance;
}

GWSSocialEnvironment::GWSSocialEnvironment() : GWSEnvironment(){
    qInfo() << "SocialEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSSocialEnvironment::~GWSSocialEnvironment(){
}


/**********************************************************************
 GETTERS
**********************************************************************/

QStringList GWSSocialEnvironment::getOutRelationeds(QSharedPointer<GWSAgent> agent, QString relation_type){
    QStringList agent_uids;
    foreach (GWSSocialRelation rel , this->out_relations.value( relation_type ).value( agent->getUID() ) ) {
        //agent_uids.append( rel.to_agent_uid );
    }
    return agent_uids;
}


QStringList GWSSocialEnvironment::getInRelationeds(QSharedPointer<GWSAgent> agent, QString relation_type){
    QStringList agent_uids;
    foreach (GWSSocialRelation rel , this->in_relations.value( relation_type ).value( agent->getUID() ) ) {
        //agent_uids.append( rel.from_agent_uid );
    }
    return agent_uids;
}
