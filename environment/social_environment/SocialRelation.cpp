#include "SocialRelation.h"

QString GWSSocialRelation::AGENT_FROM_PROP = "from_agent";
QString GWSSocialRelation::AGENT_TO_PROP = "to_agent";
QString GWSSocialRelation::SOCIAL_RELATION_PROP = "relation";

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSSocialRelation::getFromNodeUID() const{
    return this->from_agent_uid;
}

QString GWSSocialRelation::getToNodeUID() const{
    return this->to_agent_uid;
}
