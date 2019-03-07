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
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../util/parallelism/ParallelismController.h"
#include "../../util/io/log/Logger.h"

GWSApp* GWSApp::globalInstance(int argc, char *argv[]){
    // int &argc, char *argv[]
    static GWSApp instance(argc , argv);
    return &instance;
}

GWSApp::GWSApp(int argc, char* argv[]) : QCoreApplication( argc , argv ) , created_timestamp( QDateTime::currentMSecsSinceEpoch() ) {

    // READ CONFIGURATION (FILE OR JSON)
    QJsonParseError jerror;
    QStringList splitted = QString( argv[ argc-1 ] ).split('=');
    if( splitted[0] == "config" ){
        qDebug() << "Found configuration JSON" << splitted[1].toUtf8();
        this->json_configuration = QJsonDocument::fromJson( splitted[1].toUtf8() , &jerror ).object();
    }
    if( splitted[0] == "config_file" ){
        qDebug() << "Found configuration FILE" << splitted[1].toUtf8();
        QFile file( splitted[1] );
        file.open( QFile::ReadOnly );
        this->json_configuration = QJsonDocument::fromJson( file.readAll() , &jerror ).object();
    }

    if( this->json_configuration.isEmpty() || jerror.error != QJsonParseError::NoError ){
        qCritical() << QString("Error when parsing configuration JSON: %1").arg( jerror.errorString() );
        this->exit( -1 );
    }

    Q_ASSERT( !this->getAppId().isEmpty() );
    Q_ASSERT( !this->getUserId().isEmpty() );

    // Redirect outputs to file
    if( this->property("console").toBool() ){
        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= 2 ){
                GWSLogger::log( QString("[%1 - %2] %3").arg( QDateTime::currentDateTime().toString() ).arg( type ).arg( msg ) , GWSApp::globalInstance()->getAppId() ); Q_UNUSED( context );
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
