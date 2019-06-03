#include "TransactionBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

QString TransactionBehaviour::EMITTING_ENTITY_ID = "emitting_entity_id";
QString TransactionBehaviour::RECEIVING_ENTITY_ID = "receiving_entity_id";
QString TransactionBehaviour::TRANSACTION_TYPE = "transaction_type";
QString TransactionBehaviour::TRANSACTION_DATA = "transaction_data";
QString TransactionBehaviour::NEXTS = "nexts";


TransactionBehaviour::TransactionBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > TransactionBehaviour::behave(){

    QSharedPointer< GWSEntity > emitter;
    QSharedPointer< GWSEntity > receiver;

    // Get emitter
    emitter = GWSEntityEnvironment::globalInstance()->getByClassAndUID( GWSEntity::staticMetaObject.className() , this->getProperty( EMITTING_ENTITY_ID ).toString() );

    // Get receiver
    receiver = GWSEntityEnvironment::globalInstance()->getByClassAndUID( GWSEntity::staticMetaObject.className() , this->getProperty( RECEIVING_ENTITY_ID ).toString() );

    if( emitter.isNull() || receiver.isNull() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

    }


    QJsonObject transaction_data = this->getProperty( TRANSACTION_DATA ).toObject();

    foreach( QString key , transaction_data.keys() ){

           // If it comes between '<>', it is not the property name, but a kew to fetch said property name from one agent's value
           if( key.startsWith("<") && key.endsWith(">") ){
               QString property_key = key.remove( 0 , 1 );
               property_key = property_key.remove( property_key.length() - 1 , 1 );
               key = emitter->getProperty( property_key ).toString();
           }

           QJsonValue increment = transaction_data.value( key );

           // If it comes between '<>', it is not a value, but a key to fetch that property from the agent
           if( increment.toString().startsWith("<") && increment.toString().endsWith(">") ){
               QString property_name = increment.toString().remove( 0 , 1 );
               property_name = property_name.remove( property_name.length() - 1 , 1 );
               increment = emitter->getProperty( property_name );
           }

           QJsonValue receiver_existing_value = receiver->getProperty( key );
           QJsonValue values_sum = GWSObjectFactory::incrementValue( receiver_existing_value , increment );

           receiver->setProperty( key , values_sum );
           emitter->setProperty( key , QJsonValue() );

           // Store transfers log
           QJsonObject transaction;

           QString id = QString("%1%2%3").arg( emitter->getUID() ).arg( receiver->getUID() ).arg( emitter->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP).toString() );
           transaction.insert( GWSObject::GWS_UID_PROP , id );

           transaction.insert( GWSObject::GWS_CLASS_PROP , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );
           transaction.insert( "type" , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );

           transaction.insert( "refEmitter" , emitter->getUID() );

           transaction.insert( "refReceiver" , receiver->getUID() );
           transaction.insert( "geometry" , emitter->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
           transaction.insert( "time" , emitter->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ) );

           transaction.insert( key , values_sum );

           emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( transaction );

       }


    // Set a counter of how many transactions have there been towards the corresponding receiver:
     int transaction_count = receiver->getProperty( "transaction_count" ).toInt();
     transaction_count = transaction_count + 1;
     receiver->setProperty("transaction_count", transaction_count );

       return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
   }
