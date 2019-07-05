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
QString TransactionBehaviour::PROPERTY_NAMES_TO_TRANSFER = "property_names_to_transfer";
QString TransactionBehaviour::TRANSACTION_TYPE = "transaction_type";
QString TransactionBehaviour::TRANSACTION_DATA = "transaction_data";
QString TransactionBehaviour::NEXTS = "nexts";


TransactionBehaviour::TransactionBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > TransactionBehaviour::behave(){

    QSharedPointer< GWSEntity > emitter;
    QSharedPointer< GWSEntity > receiver;

    // Get emitter
    emitter = GWSEntityEnvironment::globalInstance()->getByUID( this->getProperty( EMITTING_ENTITY_ID ).toString() );

    // Get receiver
    receiver = GWSEntityEnvironment::globalInstance()->getByUID( this->getProperty( RECEIVING_ENTITY_ID ).toString() );
    qDebug() << emitter << this->getProperty( RECEIVING_ENTITY_ID  );

    if( emitter.isNull() || receiver.isNull() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    // Notify the transaction
    QJsonObject transaction = this->getProperty( TRANSACTION_DATA ).toObject();

    QDateTime datetime = QDateTime::fromMSecsSinceEpoch( emitter->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP).toDouble() );

    QString id = QString("%1%2%3").arg( emitter->getUID() ).arg( receiver->getUID() ).arg( datetime.toString( Qt::ISODate ) );
    transaction.insert( GWSObject::GWS_UID_PROP , id );

    transaction.insert( GWSObject::GWS_CLASS_PROP , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );
    transaction.insert( "type" , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );

    transaction.insert( "emitter_uid" , emitter->getUID() );
    transaction.insert( "emitter_type" , emitter->getProperty("type") );
    transaction.insert( "emitter_geometry" , GWSGeometry( emitter->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() ).getGeoJSON() );

    transaction.insert( "receiver_uid" , receiver->getUID() );
    transaction.insert( "receiver_type" , receiver->getProperty("type") );
    transaction.insert( "receiver_geometry" , GWSGeometry( receiver->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() ).getGeoJSON() );

    //transaction.insert( "geometry" , emitter->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    transaction.insert( "time" , emitter->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ) );

    // Perform the transaction
    QJsonArray property_names = this->getProperty( PROPERTY_NAMES_TO_TRANSFER ).toArray();
    foreach (QJsonValue v , property_names ) {
        QString property_name = v.toString();
        QJsonValue to_be_set_in_receiver = GWSObjectFactory::incrementValue( receiver->getProperty( property_name ) , emitter->getProperty( property_name ) );
        emitter->setProperty( property_name , QJsonValue() );
        receiver->setProperty( property_name , to_be_set_in_receiver );
    }

    // Increment emitter transaction_count
    emitter->setProperty("emitted_transaction_count", emitter->getProperty( "emitted_transaction_count" ).toInt( 0 ) + 1 );

    // Increment receiver transaction_count
    receiver->setProperty("received_transaction_count", receiver->getProperty( "received_transaction_count" ).toInt( 0 ) + 1 );

    // Check if need to send snapshot
    quint64 snapshots = this->getProperty( GWSBehaviour::SNAPSHOT_EVERY_TICKS ).toDouble( 0 );
    if( snapshots > 0 && this->getTickedAmount() % snapshots == 0 ){
        emit GWSCommunicationEnvironment::globalInstance()->sendEntitySignal( transaction );
        emit GWSCommunicationEnvironment::globalInstance()->sendEntitySignal( emitter->serialize() );
        emit GWSCommunicationEnvironment::globalInstance()->sendEntitySignal( receiver->serialize() );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
