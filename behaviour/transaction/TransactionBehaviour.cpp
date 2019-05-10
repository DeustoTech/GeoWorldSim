#include "TransactionBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

QString TransactionBehaviour::EMITTING_AGENT_ID = "emitting_agent_id";
QString TransactionBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransactionBehaviour::TRANSACTION_TYPE = "transaction_type";
QString TransactionBehaviour::TRANSACTION_DATA = "transaction_data";
QString TransactionBehaviour::NEXTS = "nexts";


TransactionBehaviour::TransactionBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > TransactionBehaviour::behave(){

    QSharedPointer< GWSAgent > emitter;
    QSharedPointer< GWSAgent > receiver;

    // Get emitter
    emitter = GWSAgentEnvironment::globalInstance()->getByClassAndUID( GWSAgent::staticMetaObject.className() , this->getProperty( EMITTING_AGENT_ID ).toString() );

    // Get receiver
    receiver = GWSAgentEnvironment::globalInstance()->getByClassAndUID( GWSAgent::staticMetaObject.className() , this->getProperty( RECEIVING_AGENT_ID ).toString() );

    if( emitter.isNull() || receiver.isNull() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

    }

    QJsonObject transaction_data = this->getProperty( TRANSACTION_DATA ).toObject();

    foreach( QString key , transaction_data.keys() ){

        QJsonValue value = transaction_data.value( key );

        // If it comes between '<>', it is not the property name, but a key to fetch that property from the agent
        if( value.toString().startsWith("<") && value.toString().endsWith(">") ){
            QString property_name = value.toString().remove( 0 , 1 );
            property_name = property_name.remove( property_name.length() - 1 , 1 );
            value = emitter->getProperty( property_name );
        }

        QJsonValue receiver_existing_value = receiver->getProperty( key );
        QJsonValue values_sum;

        switch ( receiver_existing_value.type() ) {
            case QJsonValue::Double : {
                values_sum = value.toDouble() + receiver_existing_value.toDouble();
                break;
            }
            case QJsonValue::String : {
                values_sum = value.toString() + receiver_existing_value.toString();
                break;
            }
            case QJsonValue::Bool : {
                values_sum = value.toBool() + receiver_existing_value.toBool();
                break;
            }
            case QJsonValue::Array : {
                QJsonArray existing_array = receiver_existing_value.toArray();
                existing_array.append( value );
                values_sum = existing_array;
                break;
            }
            case QJsonValue::Object : {
                QJsonObject existing_object = receiver_existing_value.toObject();
                QJsonObject delta = value.toObject();
                foreach( QString key , delta.keys() ){
                    existing_object.insert( key , this->incrementQJsonValue( existing_object[key] , delta[key] ) );
                }

                values_sum = existing_object;
                break;
            }
            case QJsonValue::Null :
            default : {
                values_sum = value;
                break;
            }
        }

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

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}


QJsonValue TransactionBehaviour::incrementQJsonValue( QJsonValue existing_value , QJsonValue increment ){

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
