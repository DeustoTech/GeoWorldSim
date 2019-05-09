#include "PropertyHeatmapBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/Coordinate.h"


QString PropertyHeatmapBehaviour::PROPERTY_TO_HEATMAP = "property_to_heatmap";
QString PropertyHeatmapBehaviour::AGENTS_TYPE = "agents_type";
QString PropertyHeatmapBehaviour::AGENTS_FILTER = "agents_filter";
QString PropertyHeatmapBehaviour::HEATMAP_DATA = "store_heatmap_data_as";
QString PropertyHeatmapBehaviour::NEXTS = "nexts";


PropertyHeatmapBehaviour::PropertyHeatmapBehaviour() : GWSBehaviour ()
{

}


QPair< double , QJsonArray >  PropertyHeatmapBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString agents_type = this->getProperty( AGENTS_TYPE ).toString();

    QJsonObject agents_filter = this->getProperty( AGENTS_FILTER ).toObject();

    QList< QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( agents_type );

    QString property_name_to_heatmap = this->getProperty( PROPERTY_TO_HEATMAP ).toString();

    QMap< GWSCoordinate , double > heatmap_data;

    foreach (QSharedPointer<GWSAgent> agent , agents) {

        bool valid = true;

        if ( property_name_to_heatmap.isNull() ){
            valid = false;
            continue;
        }

        foreach( QString key , agents_filter.keys() ){
           // qDebug() << key << agents_filter.value( key ) << agent->getProperty( key );

            if ( ( agent->getProperty( key ).isNull() ) || ( agent->getProperty( key ) != agents_filter.value( key ) ) ){
                valid = false;
                break;
            }
        }

        if( !valid ){ continue; }

        double value_to_heatmap = agent->getProperty(  property_name_to_heatmap ).toDouble();
        qDebug() << "Value" << value_to_heatmap;
        GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        GWSCoordinate agent_coor = agent_geom.getCentroid();

        // If agents share coor, sum values:
        if ( heatmap_data.contains( agent_coor )){

            double previous_value = heatmap_data.value( agent_coor );
            value_to_heatmap = previous_value + value_to_heatmap;
            heatmap_data[ agent_coor ] = value_to_heatmap;
            continue;

        }

        heatmap_data.insert( agent_coor , value_to_heatmap );
    }

   // agent->setProperty( this->getProperty( HEATMAP_DATA ).toString( "heatmap_data" ) , heatmap_data );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
