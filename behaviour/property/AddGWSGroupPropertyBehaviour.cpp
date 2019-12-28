#include "AddGWSGroupPropertyBehaviour.h"

QString geoworldsim::behaviour::AddGWSGroupPropertyBehaviour::PROPERTY_TO_ADD_TO_GWSGROUP = "property_to_add_to_gwsgroup";
QString geoworldsim::behaviour::AddGWSGroupPropertyBehaviour::NEXTS = "nexts";


geoworldsim::behaviour::AddGWSGroupPropertyBehaviour::AddGWSGroupPropertyBehaviour() : Behaviour(){
}


/************************************************************
 * SLOTS
 ***********************************************************/

QPair< double , QJsonArray > geoworldsim::behaviour::AddGWSGroupPropertyBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();

    QString property_to_add = this->getProperty( PROPERTY_TO_ADD_TO_GWSGROUP ).toString();

    QJsonArray gws_group = agent->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray();

    gws_group.append( property_to_add );

    agent->setProperty( GWS_INHERITANCE_FAMILY_PROP , gws_group );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
