#include "CreateChildBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../object/ObjectFactory.h"

QString CreateChildBehaviour::NEXTS = "nexts";

CreateChildBehaviour::CreateChildBehaviour()
{

}


/*************************************************************************************************************************************
   SLOTS
*************************************************************************************************************************************/
QJsonArray CreateChildBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString couple_id = agent->getProperty( "couple_id" ).toString();
    QSharedPointer<GWSAgent> couple = GWSAgentEnvironment::globalInstance()->getByUID( couple_id );


    // Pass parent information as basic characteristics:
    QJsonObject new_born_json = agent->serialize();
    QDateTime next_year = QDateTime::fromMSecsSinceEpoch( GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent ) ).addYears( 1 );
    new_born_json.insert( GWSExecutionEnvironment::AGENT_BIRTH_PROP , next_year.toMSecsSinceEpoch() );
    new_born_json.remove( GWSAgent::GWS_UID_PROP );

    new_born_json.remove( "couple_id" );
    new_born_json.insert( "parent1", agent->getUID() );
    new_born_json.insert( "parent2", couple_id );
    new_born_json.remove( "children" );

    // Add behaviours
    QList< QSharedPointer<GWSBehaviour> > behaviours = agent->getBehaviours( GWSBehaviour::staticMetaObject.className() );
    if ( !behaviours.isEmpty() ) {
        QJsonArray arr;
        foreach ( QSharedPointer<GWSBehaviour> o , behaviours ){
                arr.append( o->serialize() );
        }
        new_born_json.insert( "@behaviours" , arr );
    }

    // Register through object factory:
    QSharedPointer<GWSAgent> new_born = GWSObjectFactory::globalInstance()->fromJSON( new_born_json ).dynamicCast<GWSAgent>();

    QJsonArray arr = new_born->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray();
    arr.append( "CHILD" );
    new_born->setProperty( GWS_INHERITANCE_FAMILY_PROP , arr );

    QList< QSharedPointer<GWSAgent> > children ;

    if ( !agent->getProperty( "children" ).toArray().isEmpty() ){
        children = GWSAgentEnvironment::globalInstance()->getByUIDS( agent->getProperty( "children"  ).toArray() );
    }

    QJsonArray children_ids;
    foreach( QSharedPointer<GWSAgent> child , children ){
        children_ids.append( child->getUID() );
    }

    children_ids.append( new_born->getUID() );

    agent->setProperty( "children" , children_ids );
    couple->setProperty( "children" , children_ids );

    return this->getProperty( NEXTS ).toArray();

}
