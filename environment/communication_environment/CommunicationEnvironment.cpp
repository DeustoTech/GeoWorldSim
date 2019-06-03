#include "CommunicationEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSCommunicationEnvironment* GWSCommunicationEnvironment::globalInstance(){
    static GWSCommunicationEnvironment instance;
    return &instance;
}

GWSCommunicationEnvironment::GWSCommunicationEnvironment() : GWSEnvironment(){
    qInfo() << "CommunicationEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );

    // Create global Simulation Socket
    QString socket_id = GWSApp::globalInstance()->getAppId();
    this->publishers.insert( socket_id , new GWSExternalPublisher( socket_id ) );

    // Connect signals and slots
    this->connect( this , &GWSCommunicationEnvironment::listenSocket , this , &GWSCommunicationEnvironment::connectExternalSocket );
    this->connect( this , &GWSCommunicationEnvironment::sendAgentSignal , this , &GWSCommunicationEnvironment::sendAgent );
    this->connect( this , &GWSCommunicationEnvironment::sendMessageSignal , this , &GWSCommunicationEnvironment::sendMessage );

}

GWSCommunicationEnvironment::~GWSCommunicationEnvironment(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSCommunicationEnvironment::registerEntity( QSharedPointer<GWSEntity> agent ){
    Q_UNUSED(agent);
}

void GWSCommunicationEnvironment::unregisterEntity( QSharedPointer<GWSEntity> agent ){
    Q_UNUSED(agent);
}

void GWSCommunicationEnvironment::connectExternalSocket(QString socket_id){
    GWSExternalListener* listener = this->listeners.value( socket_id , 0 );
    if( !listener ){
        qDebug() << QString("Creating external listener %1").arg( socket_id );
        listener = new GWSExternalListener( socket_id );
        this->listeners.insert( socket_id , listener );
        listener->connect( listener , &GWSExternalListener::dataReceivedSignal , this , &GWSCommunicationEnvironment::dataReceivedFromSocket );
        }
}

void GWSCommunicationEnvironment::disconnectExternalSocket(QString socket_id){
    GWSExternalListener* listener = this->listeners.value( socket_id , 0 );
    if( listener ){
        listener->disconnect( listener , &GWSExternalListener::dataReceivedSignal , this , &GWSCommunicationEnvironment::dataReceivedFromSocket );
    }
}


/**********************************************************************
 LISTENERS
**********************************************************************/

/*void GWSCommunicationEnvironment::externalEnvironmentReceived(QJsonObject json_data){

    qDebug() << "MESSAGE RECEIVED";
    emit this->dataReceivedSignal( json_data , "demo" );

    // RECEIVED AN AGENT
    if( json_data.value("signal") == "entity" ){
        json_data = json_data.value("body").toObject();

        QString type = json_data.value( GWSAgent::GWS_TYPE_PROP ).toString();
        QString id = json_data.value( GWSAgent::GWS_ID_PROP ).toString();
        bool alive = json_data.value( GWSAgent::ALIVE_PROP ).toBool();

        QSharedPointer<GWSAgent> agent;

        if( !type.isEmpty() && !id.isEmpty() ){
            agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( type , id );
        }

        if( agent && !alive ){
            agent->deleteLater();
            return;
        }

        if( agent && alive ){
            agent->deserialize( json_data );
            return;
        }

        if( !agent && alive ){
            agent = GWSObjectFactory::globalInstance()->fromJSON( json_data ).dynamicCast<GWSAgent>();
        }

    }
}*/


/**********************************************************************
 DATA
**********************************************************************/

void GWSCommunicationEnvironment::sendAgent(QJsonObject agent_json, QString socket_id){
    this->sendData( "entity" , agent_json , socket_id );
}

void GWSCommunicationEnvironment::sendMessage(QJsonObject message_json, QString socket_id){
    this->sendData( "message" , message_json , socket_id );
}

void GWSCommunicationEnvironment::sendData(QString signal , QJsonObject data , QString socket_id ){

    // To be done in the main thread
    QTimer::singleShot( 0 , GWSApp::globalInstance() , [ this , signal , data , socket_id ](){
        GWSExternalPublisher* publisher = this->publishers.value( socket_id , 0 );
        if( !publisher ){
            qDebug() << QString("Creating external publisher %1").arg( socket_id );
            publisher = new GWSExternalPublisher( socket_id );
            this->publishers.insert( socket_id , publisher );
        }
        publisher->sendMessage( signal , data );
    });
}

void GWSCommunicationEnvironment::dataReceivedFromSocket( QJsonObject data ){

    // Dynamic cast sender() method to convert a QObject* into a GWSExternalListener*
    GWSExternalListener* listener = dynamic_cast<GWSExternalListener*>( sender() );

    // Get socketID corresponding to listener from listener QMap
    QString socketUID =  this->listeners.key( listener );

    emit this->dataReceivedSignal( data , socketUID );
}
