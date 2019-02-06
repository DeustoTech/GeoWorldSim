#include "ListenToMessagesBehaviour.h"
#include "../../app/App.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../util/distributed/ExternalListener.h"

QString ListenToMessagesBehaviour::NEXTS = "nexts";

ListenToMessagesBehaviour::ListenToMessagesBehaviour() : GWSBehaviour (){

}



/**********************************************************************
 SLOTS
**********************************************************************/

void ListenToMessagesBehaviour::listen( QJsonObject message_json , QString socket_id ){
        qDebug() << "MESSAGE RECEIVED" << message_json;
        this->received_pending_messages.append( message_json );
}



QJsonArray ListenToMessagesBehaviour::behave(){

    qDebug() << "STARTING TO LISTEN";

       QString socket_id  = "ander";

        emit GWSCommunicationEnvironment::globalInstance()->listenSocket( socket_id );

       // Take the slot to the .h file so it can be used both in the disconnect and connect methods.
       this->disconnect( GWSCommunicationEnvironment::globalInstance() , &GWSCommunicationEnvironment::dataReceivedSignal , this , &ListenToMessagesBehaviour::listen);
       this->connect( GWSCommunicationEnvironment::globalInstance() , &GWSCommunicationEnvironment::dataReceivedSignal , this , &ListenToMessagesBehaviour::listen);

        if ( !this->received_pending_messages.isEmpty() ){

             qDebug() << "Pending messages:" << this->received_pending_messages;
        }


    return this->getProperty( NEXTS).toArray();

}
