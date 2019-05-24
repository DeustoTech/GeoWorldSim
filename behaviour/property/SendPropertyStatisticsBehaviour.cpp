#include "SendPropertyStatisticsBehaviour.h"

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../util/grid/Grid.h"

QString SendPropertyStatisticsBehaviour::AGENTS_TYPE = "agents_type";
QString SendPropertyStatisticsBehaviour::AGENTS_FILTER = "agents_filter";
QString SendPropertyStatisticsBehaviour::AGENTS_PROPERTY_NAME = "agents_property_name";
QString SendPropertyStatisticsBehaviour::GRID_TYPE = "grid_type";
QString SendPropertyStatisticsBehaviour::STORE_AS = "store_as";
QString SendPropertyStatisticsBehaviour::SOCKET_ID = "socket_id";
QString SendPropertyStatisticsBehaviour::NEXTS_IF_STILL_ALIVE = "nexts_if_still_alive_agents";
QString SendPropertyStatisticsBehaviour::NEXTS_IF_ALL_DEAD = "nexts_if_all_agents_dead";

SendPropertyStatisticsBehaviour::SendPropertyStatisticsBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > SendPropertyStatisticsBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    qint64 agent_internal_time = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    QString agents_type = this->getProperty( AGENTS_TYPE ).toString();
    QJsonObject agents_filter = this->getProperty( AGENTS_FILTER ).toObject();
    QString grid_type = this->getProperty( GRID_TYPE ).toString();

    QList< QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( agents_type );
    QList< QSharedPointer<GWSAgent> > valid_agents;

    foreach( QSharedPointer<GWSAgent> a , agents ) {

        bool valid = true;

        foreach( QString key , agents_filter.keys() ){

            if ( ( a->getProperty( key ).isNull() ) || ( a->getProperty( key ) != agents_filter.value( key ) )   ){
                valid = false;
                break;
            }
        }
        if ( valid ){
            valid_agents.append( a );
        }

    }

    if ( valid_agents.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ALL_DEAD ).toArray() );
    }


    GWSGeometry bounds = GWSPhysicalEnvironment::globalInstance()->getBounds( agents_type );
    GWSGrid grid( bounds , 100 , 100 ,  grid_type );

    unsigned int count = 0;
    QJsonValue total_sum = 0;
    double average = 0;
    qint64 grid_time = agent->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    foreach( QSharedPointer<GWSAgent> a , valid_agents ) {

        QJsonValue val = a->getProperty( this->getProperty( AGENTS_PROPERTY_NAME ).toString() );
        if( val.isNull() ){ continue; }

        qint64 agent_time = a->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( grid_time );
        if( agent_time > grid_time ){ continue; }

        // GRID
        GWSGeometry agent_geom = GWSGeometry( a->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        grid.addValue( agent_geom , val );

        // AVERAGE
        if( average == 0 ){ average = val.toDouble(); }
        average = (average + val.toDouble()) / 2;

        // TOTAL
        total_sum = GWSObjectFactory::incrementValue( total_sum , val );

        // COUNT
        count++;
    }

    agent->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_count" , (int)count );
    agent->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_sum" , total_sum );
    agent->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_avg", average );

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( GWSApp::globalInstance()->getAppId() );

    // Send the statistics
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( agent->serialize() , socket_id );

    // Send cell points
    QList<GWSCoordinate> now_sent_coordinates;
    QStringList now_sent_coordinates_ids;
    double max_value = grid.getMaxValue();
    double min_value = grid.getMinValue();
    for(unsigned int i = 0 ; i < grid.getXSize() ; i++){
        for(unsigned int j = 0 ; j < grid.getYSize() ; j++ ){

            GWSCoordinate coor = GWSCoordinate( grid.getLon( i+0.5 ) , grid.getLat( j+0.5 ) );
            QJsonValue val = grid.getValue( coor );
            QString coor_id = QString("%1-%2").arg( i ).arg( j );

            // if( val.isNull() && !this->previous_sent_coordinates.contains( coor ) ){ continue; }

            if( val.isNull() && !this->previous_sent_coordinates_ids.contains( coor_id ) ){ continue; }

            if( !val.isNull() ){
               // now_sent_coordinates.append( coor );
                now_sent_coordinates_ids.append( coor_id );
            }

            QJsonObject grid_cell;
            grid_cell.insert( GWS_UID_PROP , QString("%1-%2-%3").arg( agent->getUID() ).arg( i ).arg( j ) );
            grid_cell.insert( "type" , agent->getUID() + "Cell" );
            grid_cell.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , (qint64)agent_internal_time );

            QJsonObject geometry;
            geometry.insert( "type" , "Point" );
            QJsonArray coordinates = { coor.getX() , coor.getY() };

            geometry.insert( "coordinates" , coordinates );
            grid_cell.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , geometry );
            grid_cell.insert( "value" , val );
            double normalized = ( val.toDouble(0) - min_value) / (max_value - min_value);
            grid_cell.insert( "color" , QString("rgb(%1,92,%2)").arg( normalized * 255 ).arg( (1-normalized) * 255 ) );
            emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( grid_cell , socket_id );
        }
    }

    // Keep back reference to update the olds that should be set to null
   // this->previous_sent_coordinates = now_sent_coordinates;
    this->previous_sent_coordinates_ids = now_sent_coordinates_ids;

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_STILL_ALIVE ).toArray() );
}
