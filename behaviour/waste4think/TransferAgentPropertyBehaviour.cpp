#include "TransferAgentPropertyBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_NAME_TO_TRANSFER = "property_name_to_transfer";
QString TransferAgentPropertyBehaviour::EMITTING_AGENT_ID = "emitting_agent_id";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour(){

}

QStringList TransferAgentPropertyBehaviour::behave(){

    QSharedPointer< GWSAgent > emitter;
    QSharedPointer< GWSAgent > receiver;

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Get emitter
    emitter = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , this->getProperty( EMITTING_AGENT_ID ).toString() );

    // Get receiver
    receiver = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , this->getProperty( RECEIVING_AGENT_ID ).toString() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();

    if( emitter.isNull() || receiver.isNull() ){
        return nexts;
    }

    QString property_name_to_be_transferred = this->getProperty( PROPERTY_NAME_TO_TRANSFER ).toString();
    QVariant value_to_be_transferred = emitter->getProperty( property_name_to_be_transferred );

    QVariant existing_value = receiver->getProperty( property_name_to_be_transferred );
    QVariant values_sum;

    switch ( value_to_be_transferred.type() ) {
    case QVariant::Int :
    case QVariant::UInt :
    case QVariant::Double :
    case QVariant::LongLong :
    case QVariant::ULongLong : {
        values_sum = value_to_be_transferred.toLongLong() + existing_value.toLongLong();
        break;
    }
    case QVariant::String : {
        values_sum = value_to_be_transferred.toString() + value_to_be_transferred.toString();
        break;
    }
    default: {
        if( value_to_be_transferred.typeName() == QString("QJsonObject") ){

            QJsonObject existing_object = existing_value.toJsonObject();
            QJsonObject delta = value_to_be_transferred.toJsonObject();

            foreach( QString key , delta.keys() ){
                existing_object.insert( key , this->incrementQJsonValue( existing_object[key] , delta[key] ) );
            }

            values_sum = existing_object;
        }
    }
    }

    receiver->setProperty( this->getProperty( PROPERTY_NAME_TO_TRANSFER ).toString() , values_sum );
    emitter->setProperty( this->getProperty( PROPERTY_NAME_TO_TRANSFER ).toString() , QVariant());


    return nexts;

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
