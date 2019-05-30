#include "App.h"

#include <QHostAddress>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QBuffer>
#include <QDateTime>
#include <QException>
#include <QString>


#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/social_environment/SocialEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../util/parallelism/ParallelismController.h"
#include "../../util/io/log/Logger.h"

GWSApp* GWSApp::globalInstance(int argc, char *argv[]){
    // int &argc, char *argv[]
    static GWSApp instance(argc , argv);
    return &instance;
}

GWSApp::GWSApp(int argc, char* argv[]) : QCoreApplication( argc , argv ) , created_timestamp( QDateTime::currentMSecsSinceEpoch() ) {

    // READ CONFIGURATION FILE
    QJsonParseError jerror;
    if( argc <= 1 ){
        qCritical() << QString("No JSON configuration found");
        this->exit( -1 );
    }

    QFile file( argv[1] );
    file.open( QFile::ReadOnly );
    this->json_configuration = QJsonDocument::fromJson( file.readAll() , &jerror ).object();

    if( this->json_configuration.isEmpty() || jerror.error != QJsonParseError::NoError ){
        qCritical() << QString("Error when parsing configuration JSON: %1").arg( jerror.errorString() );
        this->exit( -1 );
    }

    Q_ASSERT( !this->getAppId().isEmpty() );
    Q_ASSERT( !this->getUserId().isEmpty() );

    // Redirect outputs to file
    if( this->getConfiguration().value("console").toInt( -1 ) >= 0 ){
        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= this->getConfiguration().value("console").toInt( -1 ) ){
                QMap<QtMsgType , QString> message_types = {{ QtDebugMsg , "Debug" },{ QtWarningMsg , "Warning" } , { QtCriticalMsg , "Critical" } , { QtFatalMsg , "Fatal" } , { QtInfoMsg , "Info" } };
                QJsonObject message;
                message.insert( GWSObject::GWS_UID_PROP , GWSApp::globalInstance()->getAppId() );
                message.insert( "type" , QString("Simulation-Log") );
                message.insert( "log_type" , message_types.value( type ) );
                message.insert( "log" , msg );
                message.insert( "simulation_time" , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
                message.insert( "version" , context.version );
                message.insert( "file" , context.file );
                message.insert( "line" , context.line );
                message.insert( "category" , context.category );
                message.insert( "function" , context.function );
                emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( message );
            }
        } );
    }

    // Init random generators
    qsrand( QDateTime::currentDateTime().time().second() );

}

GWSApp::~GWSApp(){
    this->json_configuration.remove("id"); // Set to null
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSApp::getAppId(){
    return this->json_configuration.value( "id" ).toString();
}

QString GWSApp::getUserId(){
    return this->json_configuration.value( "user_id" ).toString();
}

QJsonObject GWSApp::getConfiguration(){
    return this->json_configuration;
}

/**********************************************************************
 METHODS
**********************************************************************/

int GWSApp::exec(){

    try {
        QCoreApplication::exec(); // Real exec()
    } catch(std::exception &e){
        this->exec();
    }

    return -1;
}
