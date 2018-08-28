#include "ViewSkill.h"

QString ViewSkill::VIEWPORT_PROP = "view_geom";
QString ViewSkill::AGENT_TYPE_FIND_PROP = "view_agents_type";

#include "../../environment/physical_environment/PhysicalEnvironment.h"

ViewSkill::ViewSkill(GWSAgent *skilled_agent) : GWSSkill( skilled_agent ){

}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<GWSAgent*> ViewSkill::getViewingAgents(){

    QList<GWSAgent*> viewing_agents;

    // Agent type
    QString agent_type = this->getProperty( AGENT_TYPE_FIND_PROP ).toString();
    if( agent_type.isEmpty() ){ agent_type = GWSAgent::staticMetaObject.className(); }

    // Geom viewport
    GWSGeometry* geom = this->getProperty( VIEWPORT_PROP ).value<GWSGeometry*>();

    if( !geom ){
        return viewing_agents;
    }

    // Move viewport to agent location
    const GWSGeometry* agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() );
    GWSCoordinate agent_centroid = agent_geom->getCentroid();
    GWSCoordinate view_skill_centroid = geom->getCentroid();
    GWSCoordinate apply_movement = agent_centroid - view_skill_centroid;
    //qDebug() << geom->toString() << apply_movement.toString();
    geom->transformMove( apply_movement );
    //qDebug() << geom->toString();

    // Query viewing agents
    return GWSPhysicalEnvironment::globalInstance()->getAgentsIntersecting( geom , agent_type );
}
