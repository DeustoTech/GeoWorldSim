#include "ChangeColorRangeBehaviour.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../app/App.h"

#include <QColor>

QString ChangeColorRangeBehaviour::CAPACITY_PROP = "capacity_property";
QString ChangeColorRangeBehaviour::CONTAINER_PROP = "container_property";
QString ChangeColorRangeBehaviour::NEXTS = "nexts";

ChangeColorRangeBehaviour::ChangeColorRangeBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > ChangeColorRangeBehaviour::behave(){

    QSharedPointer< GWSAgent > agent = this->getAgent();

    // ---------------------
    // Get capacity
    // ---------------------

    double capacity = this->getProperty( CAPACITY_PROP ).toDouble();


    // -----------------------------------------------
    // Change edge color according to edge occupation
    // -----------------------------------------------

    QJsonArray inside_agent_ids = agent->getProperty( this->getProperty( CONTAINER_PROP ).toString() ).toArray();

    if ( !inside_agent_ids.isEmpty() ){
        int occupation = inside_agent_ids.size();
        double color_factor = ( capacity - occupation ) / ( capacity ) ;
        // Apply color:
         agent->setProperty( "color" , QString("rgb(%1,%2,%3)").arg( 255 * color_factor ).arg( 255 * ( 1 - color_factor ) ).arg( 0 ));

    }
    else{
        agent->setProperty( "color" , "Green" );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
