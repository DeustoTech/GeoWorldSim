#include "SendPropertyStatisticsBehaviour.h"

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"

QString SendPropertyStatisticsBehaviour::ENTITY_TYPE = "entity_type";
QString SendPropertyStatisticsBehaviour::ENTITY_FILTER = "entity_filter";
QString SendPropertyStatisticsBehaviour::ENTITY_PROPERTY_NAME = "entity_property_name";
QString SendPropertyStatisticsBehaviour::GRID_TYPE = "grid_type";
QString SendPropertyStatisticsBehaviour::STORE_AS = "store_as";
QString SendPropertyStatisticsBehaviour::SOCKET_ID = "socket_id";
QString SendPropertyStatisticsBehaviour::NEXTS_IF_STILL_ALIVE = "nexts_if_still_alive_entities";
QString SendPropertyStatisticsBehaviour::NEXTS_IF_ALL_DEAD = "nexts_if_all_entites_dead";

SendPropertyStatisticsBehaviour::SendPropertyStatisticsBehaviour() : GWSBehaviour(){
}


/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > SendPropertyStatisticsBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();
    qint64 entity_internal_time = entity->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();
    QJsonObject entity_filter = this->getProperty( ENTITY_FILTER ).toObject();
    QString grid_type = this->getProperty( GRID_TYPE ).toString();

    QList< QSharedPointer<GWSEntity> > entities = GWSEntityEnvironment::globalInstance()->getByClass( entity_type );
    QList< QSharedPointer<GWSEntity> > valid_entities;

    foreach( QSharedPointer<GWSEntity> a , entities ) {
        if( a->fulfillsFilter( entity_filter ) ){
            valid_entities.append( a );
        }
    }

    if ( valid_entities.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ALL_DEAD ).toArray() );
    }


    GWSGeometry bounds = GWSPhysicalEnvironment::globalInstance()->getBounds( entity_type );
    GWSGrid instant_grid( bounds , 100 , 100 ,  grid_type );

    // Create accumulated_grid
    if( !this->accumulated_grid ){
        this->accumulated_grid = new GWSGrid( bounds , 100 , 100 , grid_type );
    } else {
        //this->accumulated_grid->setBounds( bounds );
    }


    int agent_count = 0;
    QJsonValue acc_value;
    qint64 grid_time = entity->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    foreach( QSharedPointer<GWSEntity> a , valid_entities ) {

        QJsonValue val = a->getProperty( this->getProperty( ENTITY_PROPERTY_NAME ).toString() );
        if( val.isNull() ){ continue; }

        qint64 agent_time = a->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble( grid_time );
        if( agent_time > grid_time ){ continue; }

        // GRID
        GWSGeometry agent_geom = GWSGeometry( a->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        instant_grid.addValue( agent_geom , val );
        this->accumulated_grid->addValue( agent_geom , val );

        // TOTAL
        acc_value = GWSObjectFactory::incrementValue( acc_value , val );

        // COUNT
        agent_count++;
    }

    // AVERAGE
    double average = acc_value.toDouble() / agent_count;

    // GLOBAL ACCUMULATOR
    this->accumulated_total = GWSObjectFactory::incrementValue( this->accumulated_total , acc_value );

    entity->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_accumulated" , this->accumulated_total );
    entity->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_agent_count" , agent_count );
    entity->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_current" , acc_value );
    entity->setProperty( this->getProperty( STORE_AS ).toString( "statistics" ) + "_average", average );

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( GWSApp::globalInstance()->getAppId() );

    // Send the statistics
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( entity->serialize() , socket_id );

    // Send cell points
    QStringList now_sent_coordinates_ids;

    double instant_grid_max_value = instant_grid.getMaxValue();
    double instant_grid_min_value = instant_grid.getMinValue();

    for(unsigned int i = 0 ; i < instant_grid.getXSize() ; i++){
        for(unsigned int j = 0 ; j < instant_grid.getYSize() ; j++ ){

            GWSCoordinate coor = GWSCoordinate( instant_grid.getLon( i+0.5 ) , instant_grid.getLat( j+0.5 ) );
            QJsonValue instant_value = instant_grid.getValue( coor );
            QJsonValue accumulated_value = this->accumulated_grid->getValue( coor );
            QString coor_id = QString("%1-%2").arg( i ).arg( j );

            if( instant_value.isNull() && !this->previous_sent_coordinates_ids.contains( coor_id ) ){
                continue;
            }
            if( !instant_value.isNull() ){
                now_sent_coordinates_ids.append( coor_id );
            }

            QJsonObject grid_cell;
            grid_cell.insert( GWS_UID_PROP , QString("%1-%2-%3").arg( entity->getUID() ).arg( i ).arg( j ) );
            grid_cell.insert( "type" , entity->getUID() + "GridCell" );
            grid_cell.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , (qint64)entity_internal_time );

            QJsonObject geometry;
            geometry.insert( "type" , "Point" );
            QJsonArray coordinates = { coor.getX() , coor.getY() };

            geometry.insert( "coordinates" , coordinates );
            grid_cell.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , geometry );
            grid_cell.insert( "instant_value" , instant_value );
            grid_cell.insert( "accumulated_value" , accumulated_value );
            double normalized = ( instant_value.toDouble(0) - instant_grid_min_value) / (instant_grid_max_value - instant_grid_min_value);
            grid_cell.insert( "color" , QString("rgb(%1,92,%2)").arg( normalized * 255 ).arg( ( 1-normalized * 255 ) ) );
            emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( grid_cell , socket_id );
        }
    }

    // Keep back reference to update the olds that should be set to null
    this->previous_sent_coordinates_ids = now_sent_coordinates_ids;

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_STILL_ALIVE ).toArray() );
}
