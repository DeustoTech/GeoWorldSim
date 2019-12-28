#include "ComparePropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::ComparePropertyBehaviour::ENTITY_ID = "entity_id";
QString geoworldsim::behaviour::ComparePropertyBehaviour::ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::ComparePropertyBehaviour::VALUE_1 = "value_1";
QString geoworldsim::behaviour::ComparePropertyBehaviour::COMPARISON_OPERATOR = "comparator";
QString geoworldsim::behaviour::ComparePropertyBehaviour::VALUE_2 = "value_2";

QString geoworldsim::behaviour::ComparePropertyBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString geoworldsim::behaviour::ComparePropertyBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


geoworldsim::behaviour::ComparePropertyBehaviour::ComparePropertyBehaviour() : Behaviour(){

}


QPair< double , QJsonArray > geoworldsim::behaviour::ComparePropertyBehaviour::behave(){


    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();

    QList<QSharedPointer< Entity > > entities;

    QSharedPointer< Entity > behaving_entity = this->getEntity();

    if( behaving_entity->getUID() == entity_id ){
        entities.append( behaving_entity );
    } else {
        entities = geoworldsim::environment::EntityEnvironment::globalInstance()->getMatches< Entity >( entity_type , entity_id );
    }

    bool comparison_fulfilled = false;

    foreach( QSharedPointer< Entity > e , entities ){
        if( !e ){ continue; }

        QJsonValue value_1 = this->getProperty( VALUE_1 );
        QJsonValue value_2 = this->getProperty( VALUE_2 );

        // If any of those does not exist, asume FALSE
        if( value_1.isUndefined() || value_1.isNull() || value_2.isUndefined() || value_2.isNull() ){
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
        }

        QString comparison_operator = this->getProperty( COMPARISON_OPERATOR ).toString();

        // ==

        if ( comparison_operator == "==" ) {
            comparison_fulfilled = value_1 == value_2 ? true : false;
        }

        // !=

        if ( comparison_operator == "!=" ){
            comparison_fulfilled = value_1 != value_2 ? true : false;
        }

        // >=

        if ( comparison_operator == ">=" ){
            comparison_fulfilled = value_1.toDouble() >= value_2.toDouble() ? true : false;
        }

        // <=

        if ( comparison_operator == "<=" ){
            comparison_fulfilled = value_1.toDouble() <= value_2.toDouble() ? true : false;
        }

        // >

        if ( comparison_operator == ">" ){
            comparison_fulfilled = value_1.toDouble() > value_2.toDouble() ? true : false;
        }

        // <

        if ( comparison_operator == "<" ){
            comparison_fulfilled = value_1.toDouble() < value_2.toDouble() ? true : false;
        }

        if( !comparison_fulfilled ){
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
        }
    }

    // Evaluate the according to boolean:
    QPair< double , QJsonArray > next;
    if ( comparison_fulfilled ) {
        next =  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
    } else {
        next =  QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
    }
    return next;

}
