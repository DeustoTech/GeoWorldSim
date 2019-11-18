#include "TransactionBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

QString geoworldsim::behaviour::TransactionBehaviour::EMITTING_ENTITY_ID = "emitting_entity_id";
QString geoworldsim::behaviour::TransactionBehaviour::EMITTING_ENTITY_TYPE = "emitting_entity_type";
QString geoworldsim::behaviour::TransactionBehaviour::RECEIVING_ENTITY_ID = "receiving_entity_id";
QString geoworldsim::behaviour::TransactionBehaviour::RECEIVING_ENTITY_TYPE = "receiving_entity_type";
QString geoworldsim::behaviour::TransactionBehaviour::PROPERTY_NAMES_TO_TRANSFER = "property_names_to_transfer";
QString geoworldsim::behaviour::TransactionBehaviour::TRANSACTION_TYPE = "transaction_type";
QString geoworldsim::behaviour::TransactionBehaviour::TRANSACTION_DATA = "transaction_data";
QString geoworldsim::behaviour::TransactionBehaviour::NEXTS = "nexts";


geoworldsim::behaviour::TransactionBehaviour::TransactionBehaviour() : Behaviour(){

}

QPair< double , QJsonArray > geoworldsim::behaviour::TransactionBehaviour::behave(){

    QList< QSharedPointer< Entity > > emitters;
    QList< QSharedPointer< Entity > > receivers;
    QSharedPointer< Entity > behaving_entity = this->getEntity();

    // Get emitters
    QString emitting_entity_id = this->getProperty( EMITTING_ENTITY_ID ).toString();
    QString emitting_entity_type = this->getProperty( EMITTING_ENTITY_TYPE ).toString();
    if( behaving_entity->getUID() == emitting_entity_id ){
        emitters.append( behaving_entity );
    } else {
        emitters = environment::EntityEnvironment::globalInstance()->getMatches( emitting_entity_type , emitting_entity_id );
    }

    // Get receivers
    QString receiving_entity_id = this->getProperty( RECEIVING_ENTITY_ID ).toString();
    QString receiving_entity_type = this->getProperty( RECEIVING_ENTITY_TYPE ).toString();
    if( behaving_entity->getUID() == receiving_entity_id ){
        receivers.append( behaving_entity );
    } else {
        receivers = environment::EntityEnvironment::globalInstance()->getMatches( receiving_entity_type , receiving_entity_id );
    }

    if( emitters.isEmpty() || receivers.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    // Iterate all with all
    foreach( QSharedPointer< Entity > emitter , emitters) {
        foreach ( QSharedPointer< Entity > receiver , receivers ) {


            // Notify the transaction
            QJsonObject transaction = this->getProperty( TRANSACTION_DATA ).toObject();

            QDateTime datetime = QDateTime::fromMSecsSinceEpoch( emitter->getProperty( environment::TimeEnvironment::INTERNAL_TIME_PROP).toDouble() );

            QString id = QString("%1%2%3").arg( emitter->getUID() ).arg( receiver->getUID() ).arg( datetime.toString( Qt::ISODate ) );
            transaction.insert( Object::GWS_UID_PROP , id );

            transaction.insert( Object::GWS_CLASS_PROP , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );
            transaction.insert( "type" , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );

            transaction.insert( "emitter_id" , emitter->getUID() );
            transaction.insert( "emitter_type" , emitter->getProperty("type") );
            transaction.insert( "emitter_geometry" , geometry::Geometry( emitter->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() ).getGeoJSON() );

            transaction.insert( "receiver_id" , receiver->getUID() );
            transaction.insert( "receiver_type" , receiver->getProperty("type") );
            transaction.insert( "receiver_geometry" , geometry::Geometry( receiver->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() ).getGeoJSON() );

            //transaction.insert( "geometry" , emitter->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
            transaction.insert( environment::TimeEnvironment::INTERNAL_TIME_PROP , emitter->getProperty( environment::TimeEnvironment::INTERNAL_TIME_PROP ) );

            // Perform the transaction
            QJsonArray property_names = this->getProperty( PROPERTY_NAMES_TO_TRANSFER ).toArray();
            foreach( QJsonValue v , property_names ) {
                QString property_name = v.toString();
                QJsonValue to_be_set_in_receiver = ObjectFactory::incrementValue( receiver->getProperty( property_name ) , emitter->getProperty( property_name ) );
                emitter->setProperty( property_name , QJsonValue() );
                receiver->setProperty( property_name , to_be_set_in_receiver );
            }

            // Increment emitter transaction_count
            emitter->setProperty("emitted_transaction_count", emitter->getProperty( "emitted_transaction_count" ).toInt( 0 ) + 1 );

            // Increment receiver transaction_count
            receiver->setProperty("received_transaction_count", receiver->getProperty( "received_transaction_count" ).toInt( 0 ) + 1 );

            // Check if need to send snapshot
            quint64 snapshots = this->getProperty( Behaviour::SNAPSHOT_EVERY_TICKS ).toDouble( 0 );
            if( snapshots > 0 && this->getTickedAmount() % snapshots == 0 ){

                emit environment::CommunicationEnvironment::globalInstance()->sendEntitySignal( transaction );
                emit environment::CommunicationEnvironment::globalInstance()->sendEntitySignal( emitter->serialize() );

                if( emitter != receiver ){
                    // Send with emitters current datetime
                    QJsonObject receiver_json = receiver->serialize();
                    receiver_json.insert( environment::TimeEnvironment::INTERNAL_TIME_PROP , emitter->getProperty( environment::TimeEnvironment::INTERNAL_TIME_PROP ) );
                    emit environment::CommunicationEnvironment::globalInstance()->sendEntitySignal( receiver_json );
                }
            }

        }
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
