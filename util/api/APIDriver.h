#ifndef GWSAPIDRIVER_H
#define GWSAPIDRIVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QJsonObject>


class GWSAPIDriver : public QObject
{
    Q_OBJECT

public:

    static GWSAPIDriver* globalInstance();

    // GET
    virtual QNetworkReply* GET( QUrl url, QMap<QString, QString> headers = QMap<QString, QString>() );

    // POST
    virtual QNetworkReply* POST( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );
    virtual QNetworkReply* POST( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QJsonObject data = QJsonObject() );

    // PUT
    virtual QNetworkReply* PUT( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );
    virtual QNetworkReply* PUT( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QJsonObject data = QJsonObject() );

    // PATCH
    virtual QNetworkReply* PATCH( QUrl url , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray());

    // DELETE
    virtual QNetworkReply* DELETE( QUrl url, QMap<QString, QString> headers = QMap<QString, QString>() );

private:
    GWSAPIDriver( );
    GWSAPIDriver(const GWSAPIDriver& other);
    ~GWSAPIDriver();

    virtual QNetworkReply* operation( QNetworkAccessManager::Operation operation , QUrl url , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );

    QNetworkAccessManager* access_manager;

};

#endif // GWSAPIDRIVER_H
