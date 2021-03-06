#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>


// Include PopulationGenerator files:
//#include "header.h"

// Include GWS files:
#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../object/Object.h"
#include "../../behaviour/Behaviour.h"
#include "../../entity/Entity.h"

// Skills:
#include "../../skill/move/MoveSkill.h"

// Behaviours:
#include "../../behaviour/population/GeneratePopulationBehaviour.h"
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/time/WaitUntilTimeBehaviour.h"
#include "../../behaviour/information/SendEntitySnapshotBehaviour.h"
#include "../../behaviour/population/CreateChildBehaviour.h"

// Utils:
#include "../../util/routing/EdgeVisitor.hpp"
#include "../../util/routing/TSPRouting.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/EntityGeneratorDatasource.h"

// Environments:
#include "../../environment/Environment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"


/**************************************************************************************
  M A I N
***************************************************************************************/
int main( int argc, char* argv[] )
{

    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // INIT OBJECT FACTORY
    //GWSObjectFactory::globalInstance()->registerType( PopulationGeneratorAgent::staticMetaObject );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSEntityEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GeneratePopulationBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopEntityBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( WaitUntilTimeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendEntitySnapshotBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CreateChildBehaviour::staticMetaObject );

    // CREATE POPULATION
    QList<GWSEntityGeneratorDatasource*> pending_datasources;
    QDateTime datasource_download_time = QDateTime::currentDateTime();
    QJsonObject json_population = GWSApp::globalInstance()->getConfiguration().value("population").toObject();
     foreach( QString key , json_population.keys() ) {

         // Population type:
         QJsonObject population = json_population[ key ].toObject();

         if ( !population.value( "template" ).isNull() && !population.value( "datasources" ).isNull() ){

             QJsonArray datasources = population.value( "datasources" ).toArray();

             for ( int i = 0; i <  datasources.size() ; ++i ){

                 QJsonObject datasource = datasources.at( i ).toObject();
                 QString scenario_id = datasource.value("scenario_id").toString();
                 int limit = datasource.value("limit").toInt(-1);
                 QString entity_type = datasource.value("entity_type").toString();
                 QString entity_filter = datasource.value("entity_filter").toString();
                 if( scenario_id.isEmpty() || entity_type.isEmpty() ){
                     qWarning() << "Asked to download from scenario without ID or entity_type";
                 }

                 GWSEntityGeneratorDatasource* ds = new GWSEntityGeneratorDatasource( population.value( "template" ).toObject() , scenario_id,  entity_type , entity_filter ,  limit > 0 ? limit : 999999999999999  );
                 pending_datasources.append( ds );

                 ds->connect( ds , &GWSEntityGeneratorDatasource::dataReadingFinishedSignal , [ ds , &pending_datasources , datasource_download_time ](){
                     pending_datasources.removeAll( ds );
                     ds->deleteLater();
                     if( pending_datasources.isEmpty() ){
                         qDebug() << "Elapsed time" << QDateTime::currentDateTime().secsTo( datasource_download_time );
                         GWSExecutionEnvironment::globalInstance()->run();
                     }
                 });

             }
         }

         if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
             for ( int i = 0; i < population.value("amount").toInt() ; i++){
                 // Use template to generate amount Entitys
                 GWSObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<GWSEntity>();
             }
         }
        qDebug() << QString("Creating population %1").arg( key );
     }
     if( pending_datasources.isEmpty() ){
         GWSExecutionEnvironment::globalInstance()->run();
     }



     // LISTEN TO EXTERNAL SIMULATIONS
     // GWSExternalListener and GWSCommunicationEnvironment have changed, do the code below needs to eventually be modified:
     QJsonObject json_external_listeners = GWSApp::globalInstance()->getConfiguration().value("external_listeners").toObject();
     foreach( QString key , json_external_listeners.keys() ) {

         // Get simulation to be listened to from config.json file
         if ( !json_external_listeners[ key ].isNull() ){
             new GWSExternalListener( json_external_listeners[ key ].toString() );
         }
         qDebug() << QString("Creating external listener %1").arg( key );
      }


     app->exec();

 }






