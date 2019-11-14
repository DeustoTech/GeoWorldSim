#include "App.h"

#include <QJsonDocument>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QException>
#include <QString>

#include "../../util/network/HttpDriver.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/social_environment/SocialEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"


geoworldsim::App* geoworldsim::App::globalInstance(int argc, char *argv[]){
    // int &argc, char *argv[]
    static geoworldsim::App instance(argc , argv);
    return &instance;
}

geoworldsim::App::App(int argc, char* argv[]) : QCoreApplication( argc , argv ) , created_timestamp( QDateTime::currentMSecsSinceEpoch() ) {

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
    /*if( this->getConfiguration().value("console").toInt( -1 ) >= 0 ){

        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= geoworldsim::GWSApp::globalInstance()->getConfiguration().value("console").toInt(-1) ){
                QMap<QtMsgType , QString> message_types = {{ QtDebugMsg , "Debug" },{ QtWarningMsg , "Warning" } , { QtCriticalMsg , "Critical" } , { QtFatalMsg , "Fatal" } , { QtInfoMsg , "Info" } };
                QJsonObject message;
                message.insert( GWSObject::GWS_UID_PROP , geoworldsim::GWSApp::globalInstance()->getAppId() );
                message.insert( "type" , QString("Simulation-Log") );
                message.insert( "log_type" , message_types.value( type ) );
                message.insert( "log" , msg );
                message.insert( "time" , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
                message.insert( "version" , context.version );
                message.insert( "file" , context.file );
                message.insert( "line" , context.line );
                message.insert( "category" , context.category );
                message.insert( "function" , context.function );
                emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( message , geoworldsim::GWSApp::globalInstance()->getAppId() + "-LOG" );
            }
        } );
    }*/

    // Init API driver to get this thread as the main one
    network::HttpDriver::globalInstance();

    // Init random generators
    qsrand( QDateTime::currentDateTime().time().second() );

}

geoworldsim::App::~App(){
    this->json_configuration.remove("id"); // Set to null
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::App::getAppId(){
    return this->json_configuration.value( "id" ).toString();
}

QString geoworldsim::App::getUserId(){
    return this->json_configuration.value( "user_id" ).toString();
}

const QJsonObject& geoworldsim::App::getConfiguration(){
    return this->json_configuration;
}

/**********************************************************************
 METHODS
**********************************************************************/

int geoworldsim::App::exec(){
    qInfo() << QString("Simulation with UID %1 starting at %2").arg( this->getAppId() ).arg( QDateTime::currentDateTime().toString() );

    try {
        QCoreApplication::exec(); // Real exec()
    } catch(std::exception &e){
        geoworldsim::environment::ExecutionEnvironment::globalInstance()->stop();
        geoworldsim::environment::ExecutionEnvironment::globalInstance()->run();
        this->exec();
    }

    return -1;
}

void geoworldsim::App::exit(int retcode){
    qInfo() << QString("Simulation with UID %1 exiting at %2 with code %3").arg( this->getAppId() ).arg( QDateTime::currentDateTime().toString() ).arg( retcode );
    QCoreApplication::exit(retcode);
}
