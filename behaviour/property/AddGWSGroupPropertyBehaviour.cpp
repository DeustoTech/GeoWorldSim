#include "AddGWSGroupPropertyBehaviour.h"

QString AddGWSGroupPropertyBehaviour::PROPERTY_TO_ADD_TO_GWSGROUP = "property_to_add_to_gwsgroup";
QString AddGWSGroupPropertyBehaviour::NEXT = "nexts";


AddGWSGroupPropertyBehaviour::AddGWSGroupPropertyBehaviour()
{

}



/************************************************************
 * SLOTS
 ***********************************************************/


QJsonArray AddGWSGroupPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString property_to_add = this->getProperty( PROPERTY_TO_ADD_TO_GWSGROUP ).toString();

    QJsonArray gws_group = agent->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray();

    gws_group.append( property_to_add );

    agent->setProperty( GWS_INHERITANCE_FAMILY_PROP , gws_group );

  //  qDebug() << agent->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray();

    return this->getProperty( NEXT ).toArray();


}
