#include "MathPropertyBehaviour.h"

#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::MathPropertyBehaviour::ENTITY_ID = "entity_id";
QString geoworldsim::behaviour::MathPropertyBehaviour::ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::MathPropertyBehaviour::OPERAND_1 = "operand_1";
QString geoworldsim::behaviour::MathPropertyBehaviour::OPERAND_2 = "operand_2";
QString geoworldsim::behaviour::MathPropertyBehaviour::OPERATOR = "operator";
QString geoworldsim::behaviour::MathPropertyBehaviour::STORE_AS = "store_as";
QString geoworldsim::behaviour::MathPropertyBehaviour::NEXTS = "nexts";


geoworldsim::behaviour::MathPropertyBehaviour::MathPropertyBehaviour() : Behaviour (){

}


QPair< double , QJsonArray > geoworldsim::behaviour::MathPropertyBehaviour::behave(){

    QString entity_id = this->getProperty( ENTITY_ID ).toString();
    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();

    QList<QSharedPointer< Entity > > entities;

    QSharedPointer< Entity > behaving_entity = this->getEntity();

    if( behaving_entity->getUID() == entity_id ){
        entities.append( behaving_entity );
    }

    // If ID and Type supplied
    else if( !entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByClassAndUID( entity_type , entity_id ) );
    }

    // If only Type supplied, affect all
    else if( entity_id.isEmpty() && !entity_type.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByClass( entity_type ) );
    }

    // If only ID supplied
    else if ( !entity_id.isEmpty() ){
        entities.append( geoworldsim::environment::EntityEnvironment::globalInstance()->getByUID( entity_id ) );
    }

    foreach (QSharedPointer< Entity > e , entities ) {
        if( !e ){ continue; }

        QString operation = this->getProperty( OPERATOR ).toString();
        double operand_1 = this->getProperty( OPERAND_1 ).toDouble();
        double operand_2 = this->getProperty( OPERAND_2 ).toDouble();
        double operation_result;

        if ( operation == "+" ){
            operation_result = operand_1 + operand_2;
        }
        if ( operation == "/" ){
            operation_result = operand_1 / operand_2;
        }
        if ( operation == "-" ){
            operation_result = operand_1 - operand_2;
        }
        if ( operation == "*" ){
            operation_result = operand_1 * operand_2;
        }

        e->setProperty( this->getProperty( STORE_AS ).toString() , operation_result );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
