#include "IncrementPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::IncrementPropertyBehaviour::ENTITY_ID = "entity_to_increment_id";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::PROPERTY_NAME_PROP = "property";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::INCREMENT_VALUE_PROP = "increment";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::MAX_THRESHOLD_VALUE_PROP = "max_threshold";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::MIN_THRESHOLD_VALUE_PROP = "min_threshold";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::MAX_LIMIT_VALUE_PROP = "max_limit";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::MIN_LIMIT_VALUE_PROP = "min_limit";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::NEXTS_IF_MAX_THRESHOLD_REACHED = "nexts_if_max_reached";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::NEXTS_IF_MIN_THRESHOLD_REACHED = "nexts_if_min_reached";
QString geoworldsim::behaviour::IncrementPropertyBehaviour::NEXTS_ELSE = "nexts_else";


geoworldsim::behaviour::IncrementPropertyBehaviour::IncrementPropertyBehaviour() : Behaviour(){

}

/**********************************************************************
 SLOTS
**********************************************************************/


QPair< double , QJsonArray > geoworldsim::behaviour::IncrementPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QSharedPointer<Entity> entity = this->getEntity();

    if( entity->getUID() != entity_id ){
        entity = geoworldsim::environment::EntityEnvironment::globalInstance()->getByUID( entity_id );
    }

    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();

    QJsonValue increment = this->getProperty( INCREMENT_VALUE_PROP );
    entity->incrementProperty( property_name , increment );

    double incremented = entity->getProperty( property_name ).toDouble();

    QJsonValue max_limit = this->getProperty( MAX_LIMIT_VALUE_PROP );
    if( max_limit.isDouble() ){ incremented = qMin( max_limit.toDouble() , incremented ); }

    QJsonValue min_limit = this->getProperty( MIN_LIMIT_VALUE_PROP );
    if( min_limit.isDouble() ){ incremented = qMax( min_limit.toDouble() , incremented ); }

    entity->setProperty( property_name , incremented );

    QJsonValue max_threshold = this->getProperty( MAX_THRESHOLD_VALUE_PROP );
    if ( max_threshold.isDouble() && incremented >= max_threshold.toDouble() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_MAX_THRESHOLD_REACHED ).toArray() );
    }

    QJsonValue min_threshold = this->getProperty( MIN_THRESHOLD_VALUE_PROP );
    if ( min_threshold.isDouble() && incremented <= min_threshold.toDouble() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_MIN_THRESHOLD_REACHED ).toArray() );
    }

    // Else
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_ELSE ).toArray() );

}
