#include "FindRandomAgentBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString FindRandomAgentBehaviour::RANDOM_AGENT_TYPE = "random_agent_type";
QString FindRandomAgentBehaviour::STORE_RANDOM_AGENT_AS = "store_random_agent_as";
QString FindRandomAgentBehaviour::STORE_RANDOM_AGENT_X_AS = "store_random_agent_x_as";
QString FindRandomAgentBehaviour::STORE_RANDOM_AGENT_Y_AS = "store_random_agent_y_as";
QString FindRandomAgentBehaviour::NEXTS_IF_FOUND = "nexts_if_found";
QString FindRandomAgentBehaviour::NEXTS_IF_NOT_FOUND = "nexts_if_not_found";



FindRandomAgentBehaviour::FindRandomAgentBehaviour() : GWSBehaviour (){

}


QJsonArray FindRandomAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString agent_type = this->getProperty( RANDOM_AGENT_TYPE ).toString();

     // Get all agents of the selected type:
    QList<QSharedPointer<GWSAgent> > available_agents = GWSAgentEnvironment::globalInstance()->getByClass( agent_type ) ;

    if( available_agents.isEmpty() ){
        return this->getProperty( NEXTS_IF_NOT_FOUND ).toArray();
    }

    // Select one of them randomly:
    QSharedPointer<GWSAgent> random_pick = available_agents.at( qrand() % available_agents.size() ) ;
    QString random_pick_id = random_pick->getId();
    QSharedPointer<GWSGeometry> random_pick_geometry = GWSPhysicalEnvironment::globalInstance()->getGeometry( random_pick );

    if( !random_pick_geometry ){
        return this->getProperty( NEXTS_IF_NOT_FOUND ).toArray();
    }

    GWSCoordinate random_pick_coor = random_pick_geometry->getCentroid();

    // Store random_pick_id in agent:
    agent->setProperty( this->getProperty( STORE_RANDOM_AGENT_AS).toString() , random_pick_id );

    // Store random_pick coordinates in agent:
    agent->setProperty( this->getProperty( STORE_RANDOM_AGENT_X_AS ).toString() ,  random_pick_coor.getX() );
    agent->setProperty( this->getProperty( STORE_RANDOM_AGENT_Y_AS ).toString() ,  random_pick_coor.getY() );

    return this->getProperty( NEXTS_IF_FOUND ).toArray();

}
