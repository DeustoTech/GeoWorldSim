#include "SocialRelation.h"

QString GWSSocialRelation::EDGE_FROM_AGENT_PROP = "from_agent";
QString GWSSocialRelation::EDGE_TO_AGENT_PROP = "to_agent";

GWSSocialRelation::GWSSocialRelation() : GWSEdge() {
}

GWSSocialRelation::~GWSSocialRelation(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSSocialRelation::getFromNodeId() const{
    return this->getFromAgentId();
}

QString GWSSocialRelation::getToNodeId() const{
    return this->getToAgentId();
}

QString GWSSocialRelation::getFromAgentId() const{
    return this->getProperty( EDGE_FROM_AGENT_PROP ).toString();
}

QString GWSSocialRelation::getToAgentId() const{
    return this->getProperty( EDGE_TO_AGENT_PROP ).toString();
}
