#include "CommunicationEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../object/ObjectFactory.h"

geoworldsim::environment::CommunicationEnvironment* geoworldsim::environment::CommunicationEnvironment::globalInstance(){
    static geoworldsim::environment::CommunicationEnvironment instance;
    return &instance;
}

geoworldsim::environment::CommunicationEnvironment::CommunicationEnvironment() : Environment(){
    qInfo() << "CommunicationEnvironment created";
    EnvironmentsGroup::globalInstance()->addEnvironment( this );

    // Create global Simulation Socket
    QString socket_id = App::globalInstance()->getAppId();
    this->publishers.insert( socket_id , new network::PublisherWebSocket( socket_id ) );

    // Connect signals and slots
    this->connect( this , &geoworldsim::environment::CommunicationEnvironment::listenSocketSignal , this , &geoworldsim::environment::CommunicationEnvironment::connectExternalSocket );
    this->connect( this , &geoworldsim::environment::CommunicationEnvironment::sendEntitySignal , this , &geoworldsim::environment::CommunicationEnvironment::sendEntity );
    this->connect( this , &geoworldsim::environment::CommunicationEnvironment::sendMessageSignal , this , &geoworldsim::environment::CommunicationEnvironment::sendMessage );

}

geoworldsim::environment::CommunicationEnvironment::~CommunicationEnvironment(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::environment::CommunicationEnvironment::registerEntity( QSharedPointer<Entity> agent ){
    Q_UNUSED(agent);
}

void geoworldsim::environment::CommunicationEnvironment::unregisterEntity( QSharedPointer<Entity> agent ){
    Q_UNUSED(agent);
}

void geoworldsim::environment::CommunicationEnvironment::connectExternalSocket(const QString &socket_id){
    network::ListenerWebSocket* listener = this->listeners.value( socket_id , 0 );
    if( !listener ){
        qDebug() << QString("Creating external listener %1").arg( socket_id );
        listener = new network::ListenerWebSocket( socket_id );
        this->listeners.insert( socket_id , listener );
        listener->connect( listener , &network::ListenerWebSocket::dataReceivedSignal , this , &geoworldsim::environment::CommunicationEnvironment::dataReceivedFromSocket );
        }
}

void geoworldsim::environment::CommunicationEnvironment::disconnectExternalSocket(const QString &socket_id){
    network::ListenerWebSocket* listener = this->listeners.value( socket_id , 0 );
    if( listener ){
        listener->disconnect( listener , &network::ListenerWebSocket::dataReceivedSignal , this , &geoworldsim::environment::CommunicationEnvironment::dataReceivedFromSocket );
    }
}


/**********************************************************************
 LISTENERS
**********************************************************************/

/*void geoworldsim::environment::GWSCommunicationEnvironment::externalEnvironmentReceived(QJsonObject json_data){

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

void geoworldsim::environment::CommunicationEnvironment::sendEntity(const QJsonObject &entity_json, const QString &socket_id){
    this->sendData( "entity" , entity_json , socket_id );
}

void geoworldsim::environment::CommunicationEnvironment::sendMessage(const QJsonObject &message_json, const QString &socket_id){
    this->sendData( "message" , message_json , socket_id );
}

void geoworldsim::environment::CommunicationEnvironment::sendData(const QString &signal , const QJsonObject &data , const QString &socket_id ){

    // To be done in the main thread
    network::PublisherWebSocket* publisher = this->publishers.value( socket_id , 0 );
    if( !publisher ){
        qDebug() << QString("Creating external publisher %1").arg( socket_id );
        publisher = new network::PublisherWebSocket( socket_id );
        publisher->setObjectName( QString("%1 %2").arg( this->metaObject()->className() ).arg( socket_id ) );
        this->publishers.insert( socket_id , publisher );
    }
    publisher->sendMessage( signal , data );
}

void geoworldsim::environment::CommunicationEnvironment::dataReceivedFromSocket( QJsonObject data ){

    // Dynamic cast sender() method to convert a QObject* into a GWSExternalListener*
    network::ListenerWebSocket* listener = dynamic_cast< network::ListenerWebSocket* >( sender() );

    // Get socketID corresponding to listener from listener QMap
    QString socketUID =  this->listeners.key( listener );

    emit this->dataReceivedSignal( data , socketUID );
}
