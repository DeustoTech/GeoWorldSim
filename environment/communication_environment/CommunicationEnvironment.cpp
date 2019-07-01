#include "CommunicationEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
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
    this->connect( this , &GWSCommunicationEnvironment::listenSocketSignal , this , &GWSCommunicationEnvironment::connectExternalSocket );
    this->connect( this , &GWSCommunicationEnvironment::sendEntitySignal , this , &GWSCommunicationEnvironment::sendEntity );
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

void GWSCommunicationEnvironment::connectExternalSocket(const QString &socket_id){
    GWSExternalListener* listener = this->listeners.value( socket_id , 0 );
    if( !listener ){
        qDebug() << QString("Creating external listener %1").arg( socket_id );
        listener = new GWSExternalListener( socket_id );
        this->listeners.insert( socket_id , listener );
        listener->connect( listener , &GWSExternalListener::dataReceivedSignal , this , &GWSCommunicationEnvironment::dataReceivedFromSocket );
        }
}

void GWSCommunicationEnvironment::disconnectExternalSocket(const QString &socket_id){
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

void GWSCommunicationEnvironment::sendEntity(const QJsonObject &entity_json, const QString &socket_id){
    this->sendData( "entity" , entity_json , socket_id );
}

void GWSCommunicationEnvironment::sendMessage(const QJsonObject &message_json, const QString &socket_id){
    this->sendData( "message" , message_json , socket_id );
}

void GWSCommunicationEnvironment::sendData(const QString &signal , const QJsonObject &data , const QString &socket_id ){

    // To be done in the main thread
    GWSExternalPublisher* publisher = this->publishers.value( socket_id , 0 );
    if( !publisher ){
        qDebug() << QString("Creating external publisher %1").arg( socket_id );
        publisher = new GWSExternalPublisher( socket_id );
        publisher->setObjectName( QString("%1 %2").arg( this->metaObject()->className() ).arg( socket_id ) );
        this->publishers.insert( socket_id , publisher );
    }
    publisher->sendMessage( signal , data );
}

void GWSCommunicationEnvironment::dataReceivedFromSocket( QJsonObject data ){

    // Dynamic cast sender() method to convert a QObject* into a GWSExternalListener*
    GWSExternalListener* listener = dynamic_cast<GWSExternalListener*>( sender() );

    // Get socketID corresponding to listener from listener QMap
    QString socketUID =  this->listeners.key( listener );

    emit this->dataReceivedSignal( data , socketUID );
}
