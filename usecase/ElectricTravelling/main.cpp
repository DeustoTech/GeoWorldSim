#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QProcess>
#include <QtMath>


#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../entity/Entity.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/EntityGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/io/csv/CsvImporter.h"
#include "../../util/ai/Intelligence.h"
#include "../../util/svm/Svm.h"
#include "../../util/api/APIDriver.h"


// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/emissions/VehicleNoiseBehaviour.h"
#include "../../behaviour/information/SendEntitySnapshotBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/property/SetEntityPropertyBehaviour.h"
#include "../../behaviour/property/SendPropertyStatisticsBehaviour.h"
#include "../../behaviour/execution/CheckIfEntitiesRunningBehaviour.h"
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.h"
#include "../../behaviour/move/SetNextRouteDestinationBehaviour.h"
#include "../../behaviour/emissions/PolluteBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"

int main(int argc, char* argv[])
{

    QDateTime start = QDateTime::currentDateTime();

    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSEntityEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();
    GWSGridEnvironment::globalInstance();
    GWSAPIDriver::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendEntitySnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( SendPropertyStatisticsBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( VehicleNoiseBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopEntityBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CalculateGTAlgRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetNextRouteDestinationBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PolluteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfEntitiesRunningBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetEntityPropertyBehaviour::staticMetaObject );

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

                 GWSEntityGeneratorDatasource* ds = new GWSEntityGeneratorDatasource( population.value( "template" ).toObject() , scenario_id,  entity_type , entity_filter , limit > 0 ? limit : 999999999999999 );
                 pending_datasources.append( ds );

                 ds->connect( ds , &GWSEntityGeneratorDatasource::dataReadingFinishedSignal , [ ds , &pending_datasources , datasource_download_time ](){
                     pending_datasources.removeAll( ds );
                     ds->deleteLater();
                     if( pending_datasources.isEmpty() ){
                         qInfo() << QString("Entities data downloaded. Took %1 seconds").arg( datasource_download_time.secsTo( QDateTime::currentDateTime() ) );
                         GWSExecutionEnvironment::globalInstance()->run();
                     }
                 });

             }
         }

         if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
             for ( int i = 0; i < population.value("amount").toInt() ; i++){
                 // Use template to generate amount entities
                 GWSObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<GWSEntity>();
             }
         }
        qInfo() << QString("Creating population %1").arg( key );
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






