#ifndef GWSAPIDRIVER_H
#define GWSAPIDRIVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>

class GWSAPIDriver : public QObject
{
    Q_OBJECT

public:
    GWSAPIDriver(QObject* parent = 0);
    ~GWSAPIDriver();

    // GET
    virtual QNetworkReply* GET( QUrl url, QMap<QString, QString> headers = QMap<QString, QString>() );

    // POST
    virtual QNetworkReply* POST( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );

    // PUT
    virtual QNetworkReply* PUT( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );

    // PATCH
    virtual QNetworkReply* PATCH( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray());

    // DELETE
    virtual QNetworkReply* DELETE( QUrl url, QMap<QString, QString> headers = QMap<QString, QString>() );

protected:
    virtual QNetworkReply* operation( QNetworkAccessManager::Operation operation , QUrl url , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );

    QNetworkAccessManager* access_manager;

};

#endif // GWSAPIDRIVER_H
