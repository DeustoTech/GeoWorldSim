#include "ChangeColorRangeBehaviour.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../app/App.h"

#include <QColor>

QString ChangeColorRangeBehaviour::MIN_EDGE_CAPACITY = "min_capacity";
QString ChangeColorRangeBehaviour::MAX_EDGE_CAPACITY = "max_capacity";
QString ChangeColorRangeBehaviour::NEXTS = "nexts";

ChangeColorRangeBehaviour::ChangeColorRangeBehaviour() : GWSBehaviour(){

}


QStringList ChangeColorRangeBehaviour::behave(){

    QSharedPointer< GWSAgent > agent = this->getAgent();

    // ---------------------
    // Get min edge capacity
    // ---------------------

    QVariant min_capacity_var = this->getProperty( MIN_EDGE_CAPACITY );

    bool min_capacity_is_property = min_capacity_var.toString().startsWith( "<" ) && min_capacity_var.toString().endsWith( ">" );

    if ( min_capacity_is_property ){

        QString property_name = min_capacity_var.toString().remove( 0 , 1 );
        property_name = property_name.remove(property_name.length() - 1 , 1 );
        min_capacity_var = agent->getProperty( property_name );

    }

    double min_capacity = min_capacity_var.toDouble();


    // ---------------------
    // Get max edge capacity
    // ---------------------

    QVariant max_capacity_var = this->getProperty( MAX_EDGE_CAPACITY );

    bool max_capacity_is_property = max_capacity_var.toString().startsWith( "<" ) && max_capacity_var.toString().endsWith( ">" );

    if ( max_capacity_is_property ){

        QString property_name = max_capacity_var.toString().remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        max_capacity_var = agent->getProperty( property_name );

    }

    double max_capacity = max_capacity_var.toDouble();


    // -----------------------------------------------
    // Change edge color according to edge occupation
    // -----------------------------------------------

    QStringList inside_agent_ids = agent->getProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP ).toStringList();
    int edge_occupation = inside_agent_ids.size();

    double color_factor = ( edge_occupation - min_capacity ) / ( max_capacity - min_capacity);

   // Apply color:
    agent->setProperty( "color" , QString("rgb(%1,%2,%3)").arg( 255 * color_factor ).arg( 255 * ( 1 - color_factor ) ).arg( 0 ));



    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
