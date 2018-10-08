#include "BroadcastToHistoryBehaviour.h"
#include "../../util/datasource/DatasourceReader.h"


BroadcastToHistoryBehaviour::BroadcastToHistoryBehaviour() : GWSBehaviour()
{

}



/**********************************************************************
 GETTERS
**********************************************************************/

bool BroadcastToHistoryBehaviour::continueToNext(){

    return false;
}

/**********************************************************************
 METHODS
**********************************************************************/

bool BroadcastToHistoryBehaviour::behave(){

QSharedPointer<GWSAgent> agent = this->getAgent();

QString app_id = GWSApp::globalInstance()->getAppId();
GWSAPIDriver api_driver;
QUrl url = QString( "http://history.geoworldsim.com/api/scenario/%1/entity" ).arg( app_id );
QMap<QString, QString> headers;
QNetworkReply* reply = api_driver.POST( url, headers, app_id.toLatin1() );
//reply->connect( reply , &QNetworkReply::finished , this , &GWSDatasourceReader::dataReceived );
//qDebug() << "Broadcasting information to history website!";

return true;
}
