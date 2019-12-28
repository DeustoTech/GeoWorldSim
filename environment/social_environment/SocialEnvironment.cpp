#include "SocialEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"


geoworldsim::environment::GWSSocialEnvironment* geoworldsim::environment::GWSSocialEnvironment::globalInstance(){
    static geoworldsim::environment::GWSSocialEnvironment instance;
    return &instance;
}

geoworldsim::environment::GWSSocialEnvironment::GWSSocialEnvironment() : Environment(){
    qInfo() << "SocialEnvironment created";
    EnvironmentsGroup::globalInstance()->addEnvironment( this );
    qRegisterMetaType< social::SocialRelation >( "social::SocialRelation" ); // Register to be used in SIGNAL/SLOTS
}

geoworldsim::environment::GWSSocialEnvironment::~GWSSocialEnvironment(){
}


/**********************************************************************
 GETTERS
**********************************************************************/

QStringList geoworldsim::environment::GWSSocialEnvironment::getOutRelationeds(QSharedPointer<Entity> agent, QString relation_type){
    QStringList agent_uids;
    foreach (social::SocialRelation rel , this->out_relations.value( relation_type ).value( agent->getUID() ) ) {
        //agent_uids.append( rel.to_agent_uid );
    }
    return agent_uids;
}


QStringList geoworldsim::environment::GWSSocialEnvironment::getInRelationeds(QSharedPointer<Entity> agent, QString relation_type){
    QStringList agent_uids;
    foreach (social::SocialRelation rel , this->in_relations.value( relation_type ).value( agent->getUID() ) ) {
        //agent_uids.append( rel.from_agent_uid );
    }
    return agent_uids;
}
