#include "App.h"

#include <QHostAddress>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QStringList>
#include <QString>
#include <QFile>
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

GWSApp::GWSApp(int argc, char* argv[]) : QCoreApplication( argc , argv ){

    this->created_timestamp = QDateTime::currentMSecsSinceEpoch();

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
    this->connect( this , &GWSApp::sendAgentSignal , this , &GWSApp::sendAgent );
    this->connect( this , &GWSApp::sendAlertSignal , this , &GWSApp::sendAlert );
    this->connect( this , &GWSApp::sendDataSignal , this , &GWSApp::pushDataToSocket );

    // Start WebSocket
    this->startSocket();
}

GWSApp::~GWSApp(){
    emit this->sendAlert( 0 , "Simulation finished" , QString("Simulation %1 finished, took %2 miliseconds").arg( this->getAppId() ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ) );
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

    emit this->sendAlert( 0 , "Simulation started" , QString("Simulation %1 started, took %2 miliseconds").arg( this->getAppId() ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ) );

    try {

        QCoreApplication::exec(); // Real exec()

    } catch(std::exception &e){
        emit this->sendAlertSignal( 2 , "Simulation had an error, trying to recover." , QString("Exception : %1").arg( e.what() ) );
        this->exec();
    }

    /*QUrl url = QString( "http://history.geoworldsim.com/api/scenario/%1/message" ).arg( this->app_id );
    QNetworkRequest request = QNetworkRequest( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader , "application/json" );
    QNetworkReply* reply = this->http_manager.post( request , QJsonDocument::fromJson( "{ \"message\" : \"finished\" }" ).toJson() );
    reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );*/

    return -1;
}


/**********************************************************************
 TCP SOCLET SLOTS
**********************************************************************/

void GWSApp::startSocket(){

    qDebug() << "Trying to connect to socket";

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){

        qDebug() << QString("Simulation connected. WebSocket connected successfully to %1").arg( this->websocket.peerAddress().toString() );
        this->pushDataToSocket( "join" , QJsonObject() ); // Join socket

        if( !this->property("autorun").isNull() ){ // If Autorun

            QTimer::singleShot( 10000 , [this](){
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
    this->websocket.open( QUrl( "ws://laika.energia.deusto.es:8070" )); //ws://localhost:8070" ) );
}

/*void GWSApp::socketPushData(QJsonValue json){
    QJsonObject response;
    response.insert( "app_id" , this->app_id );
    response.insert( "data" , json );
    QByteArray bytes = QJsonDocument( response ).toJson();

    bytes = bytes.append('¶'); // Separator

    this->tcp_socket->write( bytes , bytes.size() );
    this->tcp_socket->waitForBytesWritten(1);
    this->tcp_socket->flush();
}

void GWSApp::socketReceivedData(){
    QByteArray bytes = this->tcp_socket->readAll();
    qInfo() << "Received request" << bytes;

    QJsonObject json = QJsonDocument::fromJson( bytes ).object();
    QString type = json.value("type").toString();
    QString operation = json.value("operation").toString();

    /*if( type == GWSEnvironment::globalInstance()->metaObject()->className() ){
        if( operation == "GET" ){ GWSEnvironment::globalInstance()->pushToInterfaceSignal( GWSEnvironment::globalInstance()->toJSON() ); }
        if( operation == "RUN" ){ GWSExecutionEnvironment::globalInstance()->run(); }
        if( operation == "STOP" ){ GWSExecutionEnvironment::globalInstance()->stop(); }
    }
    else
    {
        QString id = json.value("id").toString();
        GWSAgent* agent = Q_NULLPTR;
        QList<GWSAgent*> agents;
        if( id.isEmpty() ){
            agents = GWSEnvironment::globalInstance()->getByClass( type );
        } else {
            agent = GWSEnvironment::globalInstance()->getByClassAndId( type , id );
        }

        if( operation == "RUN" && agent ){ GWSExecutionEnvironment::globalInstance()->runAgent( agent ); }
        if( operation == "RUN" && !agents.isEmpty() ){ GWSExecutionEnvironment::globalInstance()->runAgents( agents ); }
        if( operation == "STOP" && agent ){ GWSExecutionEnvironment::globalInstance()->stopAgent( agent ); }
        if( operation == "STOP" && !agents.isEmpty() ){ GWSExecutionEnvironment::globalInstance()->stopAgents( agents ); }

        if( agent ){ GWSEnvironment::globalInstance()->pushToInterfaceSignal( agent->toJSON() ); }
        if( !agents.isEmpty() ){
            QJsonArray arr;
            foreach( GWSAgent* agent , agents ){ arr.append( agent->toJSON() );}
            GWSEnvironment::globalInstance()->pushToInterfaceSignal( arr );
        }
    }
}*/

/**********************************************************************
 HTTP SOCKET SLOTS
**********************************************************************/

void GWSApp::sendAgent( QJsonObject entity_json ){

    QString entity_type = entity_json.value( GWSObject::GWS_TYPE_PROP ).toString();
    QString entity_id = entity_json.value( GWSObject::GWS_ID_PROP ).toString();
    //entity_json.insert("time" , QString("%1ms").arg( GWSTimeEnvironment::globalInstance()->getCurrentDateTime() ) );
    if( entity_type.isEmpty() || entity_id.isEmpty() ){ return; }
    this->pushDataToSocket( "entity" , entity_json );
}

void GWSApp::sendAlert( int level, QString title, QString description ){

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

void GWSApp::pushDataToSocket(QString signal , QJsonValue json){

    // TO sockets.deusto.io
    QJsonObject socket_json;
    socket_json.insert("signal" , signal );
    socket_json.insert("socket_id" , this->getAppId() );
    socket_json.insert("body" , json );
    this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );



}
