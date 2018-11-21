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

    for( int i = 0 ; i < argc ; i++){
        QString arg = QString( argv[i] );
        QStringList splitted = arg.split('=');
        if( splitted.size() == 2 ){
            qDebug() << QString("Argument found : %1 = %2").arg( splitted.at(0) ).arg( splitted.at(1 ) );
            this->setProperty( splitted.at(0).toLatin1() , splitted.at(1) );
        }
    }

    Q_ASSERT( !this->getAppId().isEmpty() );
    Q_ASSERT( !this->getUserId().isEmpty() );

    // Redirect outputs to file
    if( this->property("console").toBool() ){
        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= 0 ){
                GWSLogger::log( QString("[%1 - %2] %3").arg( QDateTime::currentDateTime().toString() ).arg( type ).arg( msg ) , QString("console.txt") ); Q_UNUSED( context );
            }
        } );
    }

    // Call Parallelism controller to take this as main thread
    GWSParallelismController::globalInstance();

    // Init random generators
    qsrand( QDateTime::currentDateTime().time().second() );

    // Connect signal to socket slots
    this->connect( this , &GWSApp::sendAgentToSocketSignal , this , &GWSApp::sendAgentToSocket );
    this->connect( this , &GWSApp::sendAlertToSocketSignal , this , &GWSApp::sendAlertToSocket );
    this->connect( this , &GWSApp::sendDataToSocketSignal , this , &GWSApp::sendDataToSocket );

    // Start WebSocket
    this->startSocket();
}

GWSApp::~GWSApp(){
    emit this->sendAlertToSocket( 0 , "Simulation finished" , QString("Simulation %1 finished, took %2 miliseconds at %3x speed with %4 threads")
                                  .arg( this->getAppId() )
                                  .arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp )
                                  .arg( GWSTimeEnvironment::globalInstance()->getTimeSpeed() )
                                  .arg( QThreadPool::globalInstance()->maxThreadCount() ) );
    this->setProperty( "id" , QVariant() ); // Set to null
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSApp::getAppId(){
    return this->property( "id" ).toString();
}

QString GWSApp::getUserId(){
    return this->property( "user_id" ).toString();
}

/**********************************************************************
 METHODS
**********************************************************************/

int GWSApp::exec(){

    emit this->sendAlertToSocket( 0 , "Simulation started" , QString("Simulation %1 started, took %2 miliseconds").arg( this->getAppId() ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ) );

    try {

        QCoreApplication::exec(); // Real exec()

    } catch(std::exception &e){
        emit this->sendAlertToSocketSignal( 2 , "Simulation had an error, trying to recover." , QString("Exception : %1").arg( e.what() ) );
        this->exec();
    }

    return -1;
}


/**********************************************************************
 TCP SOCLET SLOTS
**********************************************************************/

void GWSApp::startSocket(){

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){

        qDebug() << QString("Simulation connected. WebSocket connected successfully to %1").arg( this->websocket.peerAddress().toString() );

        if( !this->property("run").isNull() ){ // If Autorun

            QTimer::singleShot( this->property("run").toInt() * 1000 , [this](){
                GWSExecutionEnvironment::globalInstance()->run();
            });

        }
    });
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);
        emit this->websocket.ping();
    });
    /*QObject::connect( this->websocket , &QWebSocket::error , [this](QAbstractSocket::SocketError error){
        qWarning() << "Error connecting websocket" << error;
        this->startSocket();
    });*/
    QObject::connect( &this->websocket , &QWebSocket::disconnected , this , &GWSApp::reconnectSocket );

    this->reconnectSocket();
}

void GWSApp::reconnectSocket(){
    this->websocket.open( QUrl( "ws://sockets.geoworldsim.com/?scenario_id=" + this->getAppId() )  );
}

/**********************************************************************
 SOCKET SLOTS
**********************************************************************/

void GWSApp::sendAgentToSocket( QJsonObject entity_json ){

    QString entity_type = entity_json.value( GWSObject::GWS_TYPE_PROP ).toString();
    QString entity_id = entity_json.value( GWSObject::GWS_ID_PROP ).toString();
    //entity_json.insert("time" , QString("%1ms").arg( GWSTimeEnvironment::globalInstance()->getCurrentDateTime() ) );
    if( entity_type.isEmpty() || entity_id.isEmpty() ){ return; }

    this->sendDataToSocket( "entity" , entity_json );
}

void GWSApp::sendAlertToSocket( int level, QString title, QString description ){

    if( level == 0 ){
        qInfo() << title << description;
    }
    if( level == 1 ){
        qWarning() << title << description;
    }
    if( level >= 2 ){
        qCritical() << title << description;
    }

    QJsonObject alert_json;
    alert_json.insert( "level" , level );
    alert_json.insert( "title" , title );
    alert_json.insert( "description" , description );
    alert_json.insert( "user_id" , this->getUserId() );
    alert_json.insert( "generator_type" , "simulation" );
    alert_json.insert( "generator_id" , this->getAppId() );

    // TO alerts.geoworldsim.com
    QUrl url = QString( "https://alerts.geoworldsim.com/api/alert" );
    QNetworkRequest request = QNetworkRequest( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader , "application/json" );
    QNetworkReply* reply = this->http_manager.post( request , QJsonDocument( alert_json ).toJson() );
    reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );
}

void GWSApp::sendDataToSocket(QString signal , QJsonValue json){

    // TO sockets.geoworldsim.com
    QJsonObject socket_json;
    socket_json.insert("signal" , signal );
    socket_json.insert("scenario_id" , this->getAppId() );
    socket_json.insert("body" , json );
    this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );

}


