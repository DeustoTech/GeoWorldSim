#include "TransferAgentPropertyBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_NAME_TO_TRANSFER = "property_name_to_transfer";
QString TransferAgentPropertyBehaviour::EMITTING_AGENT_ID = "emitting_agent_id";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::TRANSACTION_TYPE = "transaction_type";
QString TransferAgentPropertyBehaviour::TRANSACTION_DATA = "transaction_data";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour(){

}

QJsonArray TransferAgentPropertyBehaviour::behave(){

    QSharedPointer< GWSAgent > emitter;
    QSharedPointer< GWSAgent > receiver;

    // Get emitter
    emitter = GWSAgentEnvironment::globalInstance()->getByClassAndUID( GWSAgent::staticMetaObject.className() , this->getProperty( EMITTING_AGENT_ID ).toString() );

    // Get receiver
    receiver = GWSAgentEnvironment::globalInstance()->getByClassAndUID( GWSAgent::staticMetaObject.className() , this->getProperty( RECEIVING_AGENT_ID ).toString() );

    if( emitter.isNull() || receiver.isNull() ){
        return this->getProperty( NEXTS ).toArray();
    }

    QJsonValue property_name = this->getProperty( PROPERTY_NAME_TO_TRANSFER );
    QJsonValue value_to_be_transferred = emitter->getProperty( property_name.toString() );
    QJsonValue existing_value = receiver->getProperty(  property_name.toString());
    QJsonValue values_sum;

    switch ( existing_value.type() ) {
    case QJsonValue::Double : {
        values_sum = value_to_be_transferred.toDouble() + existing_value.toDouble();
        break;
    }
    case QJsonValue::String : {
        values_sum = value_to_be_transferred.toString() + value_to_be_transferred.toString();
        break;
    }
    case QJsonValue::Bool : {
        values_sum = value_to_be_transferred.toBool() + value_to_be_transferred.toBool();
        break;
    }
    case QJsonValue::Array : {
        QJsonArray existing_array = existing_value.toArray();
        existing_array.append( value_to_be_transferred );
        values_sum = existing_array;
        break;
    }
    case QJsonValue::Object : {
        QJsonObject existing_object = existing_value.toObject();
        QJsonObject delta = value_to_be_transferred.toObject();
        foreach( QString key , delta.keys() ){
            existing_object.insert( key , this->incrementQJsonValue( existing_object[key] , delta[key] ) );
        }

        values_sum = existing_object;
        break;
    }
    case QJsonValue::Null :
    default : {
        values_sum = value_to_be_transferred;
        break;
    }
    }

    receiver->setProperty( this->getProperty( PROPERTY_NAME_TO_TRANSFER ).toString() , values_sum );
    emitter->setProperty( this->getProperty( PROPERTY_NAME_TO_TRANSFER ).toString() , QJsonValue() );

    // Store transfers log
    QJsonObject transaction = this->getProperty( TRANSACTION_DATA ).toObject();
    QString id = QString("%1%2%3").arg(emitter->getUID()).arg(receiver->getUID()).arg(GWSTimeEnvironment::globalInstance()->getAgentInternalTime( emitter ));
    transaction.insert( GWSObject::GWS_UID_PROP , id );
    transaction.insert( GWSObject::GWS_CLASS_PROP , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );
    transaction.insert( "type" , this->getProperty( TRANSACTION_TYPE ).toString( "Transaction" ) );
    transaction.insert( "refEmitter" , emitter->getUID() );
    transaction.insert( "refReceiver" , receiver->getUID() );
    transaction.insert( "time" , GWSTimeEnvironment::globalInstance()->getAgentInternalTime( emitter ) );
    transaction.insert( "value" , value_to_be_transferred );
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( transaction );

    return this->getProperty( NEXTS ).toArray();
}


QJsonValue TransferAgentPropertyBehaviour::incrementQJsonValue( QJsonValue existing_value , QJsonValue increment ){

    if( existing_value.isNull() ){
        return increment;
    }

    if( increment.isNull() ){
        return existing_value;
    }

    // Unitary elements (int, double, string, bool)

    if( existing_value.isDouble() ){
        return existing_value.toDouble() + increment.toDouble();
    }

    if( existing_value.isString() ){
        return existing_value.isString() + increment.isString();
    }

    if( existing_value.isBool() ){
        return existing_value.toBool() + increment.toBool();
    }

    // Complext elements (object)

    QJsonObject result;

    if( existing_value.isObject() ){
        foreach( QString key , existing_value.toObject().keys() ){
            result.insert( key , this->incrementQJsonValue( existing_value.toObject()[key] , increment.toObject()[ key ] ) );
        }
    }

    if( increment.isObject() ){
        foreach( QString key , increment.toObject().keys() ){
            result.insert( key , this->incrementQJsonValue( existing_value.toObject()[key] , increment.toObject()[ key ] ) );
        }
    }

    return result;
}
