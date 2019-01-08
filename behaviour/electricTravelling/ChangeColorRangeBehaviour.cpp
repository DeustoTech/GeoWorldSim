#include "ChangeColorRangeBehaviour.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../app/App.h"

#include <QColor>

QString ChangeColorRangeBehaviour::MIN_EDGE_CAPACITY = "min_capacity";
QString ChangeColorRangeBehaviour::MAX_EDGE_CAPACITY = "max_capacity";
QString ChangeColorRangeBehaviour::NEXTS = "nexts";

ChangeColorRangeBehaviour::ChangeColorRangeBehaviour() : GWSBehaviour(){

}


QJsonArray ChangeColorRangeBehaviour::behave(){

    QSharedPointer< GWSAgent > agent = this->getAgent();

    // ---------------------
    // Get min edge capacity
    // ---------------------

    QJsonValue min_capacity_var = this->getProperty( MIN_EDGE_CAPACITY );
    double min_capacity = min_capacity_var.toDouble();


    // ---------------------
    // Get max edge capacity
    // ---------------------

    QJsonValue max_capacity_var = this->getProperty( MAX_EDGE_CAPACITY );
    double max_capacity = max_capacity_var.toDouble();


    // -----------------------------------------------
    // Change edge color according to edge occupation
    // -----------------------------------------------

    QJsonArray inside_agent_ids = agent->getProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP ).toArray();
    int edge_occupation = inside_agent_ids.size();

    double color_factor = ( edge_occupation - min_capacity ) / ( max_capacity - min_capacity);

   // Apply color:
    agent->setProperty( "color" , QString("rgb(%1,%2,%3)").arg( 255 * color_factor ).arg( 255 * ( 1 - color_factor ) ).arg( 0 ));

    QJsonArray nexts = this->getProperty( NEXTS ).toArray();
    return nexts;
}
