#include "ListenToMessagesBehaviour.h"
#include "../../app/App.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../util/distributed/ExternalListener.h"

QString geoworldsim::behaviour::ListenToMessagesBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::ListenToMessagesBehaviour::ListenToMessagesBehaviour() : Behaviour (){

}



/**********************************************************************
 SLOTS
**********************************************************************/

void geoworldsim::behaviour::ListenToMessagesBehaviour::listen( QJsonObject message_json , QString socket_id ){
        qDebug() << "MESSAGE RECEIVED" << message_json;
        this->received_pending_messages.append( message_json );
}



QPair< double , QJsonArray > geoworldsim::behaviour::ListenToMessagesBehaviour::behave(){

    qDebug() << "STARTING TO LISTEN";

       QString socket_id  = "ander";

        emit environment::CommunicationEnvironment::globalInstance()->listenSocket( socket_id );

       // Take the slot to the .h file so it can be used both in the disconnect and connect methods.
       this->disconnect( environment::CommunicationEnvironment::globalInstance() , &environment::CommunicationEnvironment::dataReceivedSignal , this , &ListenToMessagesBehaviour::listen);
       this->connect( environment::CommunicationEnvironment::globalInstance() , &environment::CommunicationEnvironment::dataReceivedSignal , this , &ListenToMessagesBehaviour::listen);
        if ( !this->received_pending_messages.isEmpty() ){

             qDebug() << "Pending messages:" << this->received_pending_messages;
        }


    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS).toArray() );

}
