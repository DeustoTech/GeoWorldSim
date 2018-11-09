#include "TransferAgentPropertyBehaviour.h"

#include <QVariant>
#include <QJsonObject>

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_TO_TRANSFER = "property_to_transfer";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour()
{

}

QStringList TransferAgentPropertyBehaviour::behave(){

    QSharedPointer< GWSAgent > agent = this->getAgent();
    QVariant value_to_be_transferred = this->getProperty( PROPERTY_TO_TRANSFER );
    QString closest_agent_id = agent->getProperty( this->getProperty( RECEIVING_AGENT_ID ).toString() ).toString();
    QSharedPointer< GWSAgent > closest_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , closest_agent_id );
    QVariant existing_value = closest_agent->getProperty( PROPERTY_TO_TRANSFER );
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

    closest_agent->setProperty( PROPERTY_TO_TRANSFER , values_sum );
    agent->setProperty( PROPERTY_TO_TRANSFER , QVariant());

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
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

    // Complext elements (object)

    QJsonObject result;

    if( existing_value.isObject() ){
        foreach( QString key , existing_value.toObject().keys() ){
            result.insert( key , this->incrementQJsonValue( existing_value[key] , increment[ key ] ) );
        }
    }

    if( increment.isObject() ){
        foreach( QString key , increment.toObject().keys() ){
            result.insert( key , this->incrementQJsonValue( existing_value[key] , increment[ key ] ) );
        }
    }

    return result;
}
