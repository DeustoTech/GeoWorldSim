#include "MathAgentPropertyBehaviour.h"

QString MathAgentPropertyBehaviour::OPERATOR = "math_operator";
QString MathAgentPropertyBehaviour::OPERAND_PROPERTY = "property_to_modify";
QString MathAgentPropertyBehaviour::OPERAND_VALUE = "operand_value";
QString MathAgentPropertyBehaviour::AGENT_ID_TO_OPERATE_ON = "agent_to_operate_on_id";
QString MathAgentPropertyBehaviour::NEXTS = "nexts";


#include "../../environment/agent_environment/AgentEnvironment.h"

MathAgentPropertyBehaviour::MathAgentPropertyBehaviour() : GWSBehaviour (){

}


QJsonArray MathAgentPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( AGENT_ID_TO_OPERATE_ON ).toString();
    QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getById( agent_id );

    QString operation = this->getProperty( OPERATOR ).toString();
    QJsonValue operand_value = this->getProperty( OPERAND_VALUE );
    QString property_to_modify = this->getProperty( OPERAND_PROPERTY ).toString();

    double initial_property_value = agent->getProperty( property_to_modify ).toDouble();
    double final_property_value;

    if ( operation == "+" ){
        final_property_value = initial_property_value + operand_value.toDouble();
    }
    if ( operation == "/" ){
        final_property_value = initial_property_value / operand_value.toDouble();
    }
    if ( operation == "-" ){
        final_property_value = initial_property_value - operand_value.toDouble();

    }
    if ( operation == "*" ){
        final_property_value = initial_property_value * operand_value.toDouble();
    }

    agent->setProperty( property_to_modify , final_property_value );
    return this->getProperty( NEXTS ).toArray();
}
