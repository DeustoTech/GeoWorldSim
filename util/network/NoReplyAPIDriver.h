#ifndef NOREPLYAPIDRIVER_H
#define NOREPLYAPIDRIVER_H

#include "utils/api/ASyncAPIDriver.h"

class NoReplyAPIDriver : public GWSASyncAPIDriver
{
    Q_OBJECT

public:
    NoReplyAPIDriver(QObject* parent = 0);

protected:
    virtual QNetworkReply* operation( QNetworkAccessManager::Operation operation , QUrl url , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );

};

#endif // NOREPLYAPIDRIVER_H
