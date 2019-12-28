#ifndef HTTPDRIVER_H
#define HTTPDRIVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QJsonObject>
#include <QReadWriteLock>

namespace geoworldsim {
namespace network {


class HttpDriver : public QObject
{
    Q_OBJECT

public:

    static HttpDriver* globalInstance();

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
    HttpDriver( );
    HttpDriver(const HttpDriver& other);
    ~HttpDriver();

    // HELPER CLASS
    class HttpDriverElement {
    public :
        QNetworkRequest request;
        QNetworkAccessManager::Operation operation;
        QByteArray data;
        std::function<void(QNetworkReply*)> callback;
    };

    // Returns in parameter an Unfinished reply, receiver will need to connect to finished signal
    virtual void operation( QNetworkAccessManager::Operation operation , QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers = QMap<QString, QString>() , QByteArray data = QByteArray() , QByteArray custom_operation = QByteArray() );
    virtual void executePendingOperation( HttpDriverElement* request );

    QNetworkAccessManager* access_manager = Q_NULLPTR;
    QList< HttpDriverElement* >* pending_requests;
    int current_requests_amount = 0;
    QReadWriteLock mutex;

};


}
}

#endif // HTTPDRIVER_H
