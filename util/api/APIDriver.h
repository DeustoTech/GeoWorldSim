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
#include <QReadWriteLock>

class GWSAPIDriver : public QObject
{
    Q_OBJECT

public:

    static GWSAPIDriver* globalInstance();

    // GETTERS
    int getRequestsAmount() const;

    // GET
    virtual void GET( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>() );

    // POST
    virtual void POST( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );
    virtual void POST( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>(), QJsonObject data = QJsonObject() );

    // PUT
    virtual void PUT( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray() );
    virtual void PUT( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>(), QJsonObject data = QJsonObject() );

    // PATCH
    virtual void PATCH( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>(), QByteArray data = QByteArray());

    // DELETE
    virtual void DELETE( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>() );

private:
    GWSAPIDriver( );
    GWSAPIDriver(const GWSAPIDriver& other);
    ~GWSAPIDriver();

    // HELPER CLASS
    class GWSAPIDriverElement {
    public :
        QNetworkRequest request;
        QNetworkAccessManager::Operation operation;
        QByteArray data;
        std::function<void(QNetworkReply*)> callback;
    };

    // Returns in parameter an Unfinished reply, receiver will need to connect to finished signal
    virtual void operation( QNetworkAccessManager::Operation operation , QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );
    virtual void executePendingOperation( GWSAPIDriverElement& request );

    QNetworkAccessManager* access_manager;
    QList< GWSAPIDriverElement > pending_requests;
    int current_requests_amount = 0;
    QReadWriteLock mutex;

};

#endif // GWSAPIDRIVER_H
