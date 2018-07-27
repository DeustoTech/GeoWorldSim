#include "App.h"

#include <QHostAddress>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QDateTime>

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

#include "../../util/parallelism/ParallelismController.h"
#include "../../util/io/log/Logger.h"

GWSApp* GWSApp::globalInstance(int argc, char *argv[]){
    static GWSApp instance(argc , argv);
    return &instance;
}

GWSApp::GWSApp(int argc, char* argv[]) : QCoreApplication( argc , argv ){

    this->created_timestamp = QDateTime::currentMSecsSinceEpoch();

    for( int i = 0 ; i < argc ; i++){
        QString arg = QString( argv[i] );
        if( arg.contains( "-id=" ) ){ this->app_id = arg.replace( "-id=" , "" ); }
        if( arg.contains( "-user=" ) ){ this->setProperty( "user" , arg.replace( "-user=" , "" ) ); }
        if( arg.contains( "-autorun=" ) ){ this->setProperty( "autorun" , arg.replace( "-autorun=" , "" ) == "1" ? true : false ); }
        if( arg.contains( "-console=" ) ){ this->setProperty( "console" , arg.replace( "-console=" , "" ) == "1" ? true : false ); }
        if( arg.contains( "-speed=" ) ){ this->setProperty( "speed" , arg.replace( "-speed=" , "" ).toDouble() ); }
        if( arg.contains( "-debug=" ) ){ this->setProperty( "debug" , arg.replace( "-debug=" , "" ).toInt() ); }
        if( arg.contains( "-localpath=" ) ){ this->setProperty( "localpath" , arg.replace( "-localpath=" , "" ) ); }
    }

    Q_ASSERT( !this->app_id.isEmpty() );

    // Redirect outputs to file
    if( this->property("console").toBool() ){
        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= 1 ){
                GWSLogger::log( QString("[%1 - %2] %3").arg( QDateTime::currentDateTime().toString() ).arg( type ).arg( msg ) , QString("console.txt") ); Q_UNUSED( context );
            }
        } );
    }

    // Call Parallelism controller to take this as main thread
    GWSParallelismController::globalInstance();

    // Init random generators
    qsrand( QDateTime::currentDateTime().time().second() );

    // Connect signal to socket slots
    this->connect( this , &GWSApp::pushAgentSignal , this , &GWSApp::pushAgent );
    this->connect( this , &GWSApp::pushDataSignal , this , &GWSApp::pushData );

    // Start WebSocket
    this->startSocket();
}

GWSApp::~GWSApp(){
    qInfo() << QString("Destroyed App %1, took %2 miliseconds for %3 agents").arg( this->app_id ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ).arg( GWSAgentEnvironment::globalInstance()->getAmount() );
    this->app_id = QString();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSApp::getAppId(){
    return this->app_id;
}

/**********************************************************************
 METHODS
**********************************************************************/

int GWSApp::exec(){

    qInfo() << QString("Starting App %1 execution, took %2 miliseconds for %3 agents").arg( this->app_id ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ).arg( GWSAgentEnvironment::globalInstance()->getAmount() );
    try {
        QCoreApplication::exec(); // Real exec()
    } catch(...){
        qCritical() << "App had an error, trying to recover";
        this->pushData( "message" , "Simulation had an error, trying to recover" );
        this->exec();
    }

    /*QUrl url = QString( "http://history.deusto.io/api/scenario/%1/message" ).arg( this->app_id );
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

    qDebug() << "Trying to connect socket";

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        qInfo() << "WebSocket connected successfully to" << this->websocket.peerAddress();
        this->pushData( "join" , QJsonObject() );
        this->pushData( "message" , "Simulation connected" );

        // If Autorun
        qInfo() << QString("Connected App %1, took %2 miliseconds for %3 agents").arg( this->app_id ).arg( QDateTime::currentMSecsSinceEpoch() - this->created_timestamp ).arg( GWSAgentEnvironment::globalInstance()->getAmount() );
        if( this->property("autorun").toBool() ){
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
    //QObject::connect( &this->websocket , &QWebSocket::disconnected , this , &GWSApp::startSocket );

    this->websocket.open( QUrl( "ws://sockets.deusto.io" )); //ws://localhost:8070" ) );
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

void GWSApp::pushAgent(QJsonObject entity_json){

    QString entity_type = entity_json.value( GWSObject::GWS_TYPE_PROP ).toString();
    QString entity_id = entity_json.value( GWSObject::GWS_ID_PROP ).toString();
    //entity_json.insert("time" , QString("%1ms").arg( GWSTimeEnvironment::globalInstance()->getCurrentDateTime() ) );
    if( entity_type.isEmpty() || entity_id.isEmpty() ){ return; }
    this->pushData( "entity" , entity_json );
}

void GWSApp::pushData(QString signal , QJsonValue json){

    // TO sockets.deusto.io
    QJsonObject socket_json;
    socket_json.insert("signal" , signal );
    socket_json.insert("socket_id" , this->app_id);
    socket_json.insert("body" , json );
    this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );

    // TO history.deusto.io
    /*QUrl url = QString( "http://history.deusto.io/api/scenario/%1/entity/%2/%3" ).arg( this->app_id ).arg( entity_type ).arg( entity_id );
    QNetworkRequest request = QNetworkRequest( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader , "application/json" );
    QNetworkReply* reply = this->http_manager.post( request , QJsonDocument( json ).toJson() );
    reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );*/

}
