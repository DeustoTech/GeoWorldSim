#ifndef GWSASYNCAPIDRIVER_H
#define GWSASYNCAPIDRIVER_H

#include "utils/api/APIDriver.h"

class GWSASyncAPIDriver : public GWSAPIDriver
{
    Q_OBJECT

public:
    GWSASyncAPIDriver( QObject* parent = 0 );

protected:
    virtual QNetworkReply* operation( QNetworkAccessManager::Operation operation , QUrl url , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );

};

#endif // GWSASYNCAPIDRIVER_H
