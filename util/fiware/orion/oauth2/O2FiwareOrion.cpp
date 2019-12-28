#include "O2FiwareOrion.h"
#include "o0globals.h"
#include <QUrl>
#include <QUrlQuery>

static const char *OrionEndpoint = "https://account.lab.fiware.org/";
static const char *OrionTokenUrl = "https://account.lab.fiware.org/token";
static const char *OrionExpiresKey = "expires";

O2FiwareOrion::O2FiwareOrion(QObject *parent): O2(parent) {
    this->setRequestUrl(OrionEndpoint);
    this->setTokenUrl(OrionTokenUrl);
}

void O2FiwareOrion::onVerificationReceived(const QMap<QString, QString> response) {
    qDebug() << "O2FiwareOrion::onVerificationReceived: Emitting closeBrowser()";
    Q_EMIT closeBrowser();

    if (response.contains("error")) {
        qWarning() << "O2FiwareOrion::onVerificationReceived: Verification failed";
        foreach (QString key, response.keys()) {
            qWarning() << "O2FiwareOrion::onVerificationReceived:" << key << response.value(key);
        }
        Q_EMIT linkingFailed();
        return;
    }

    // Save access code
    setCode(response.value(O2_OAUTH2_GRANT_TYPE_CODE));

    // Exchange access code for access/refresh tokens
    QUrl url(tokenUrl_);
    QUrlQuery query(url);
    query.addQueryItem(O2_OAUTH2_CLIENT_ID, clientId_);
    query.addQueryItem(O2_OAUTH2_CLIENT_SECRET, clientSecret_);
    query.addQueryItem(O2_OAUTH2_SCOPE, scope_);
    query.addQueryItem(O2_OAUTH2_GRANT_TYPE_CODE, code());
    query.addQueryItem(O2_OAUTH2_REDIRECT_URI, redirectUri_);
    url.setQuery(query);

    QNetworkRequest tokenRequest(url);
    QNetworkReply *tokenReply = manager_->get(tokenRequest);
    timedReplies_.add(tokenReply);
    connect(tokenReply, SIGNAL(finished()), this, SLOT(onTokenReplyFinished()), Qt::QueuedConnection);
    connect(tokenReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onTokenReplyError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
}

void O2FiwareOrion::onTokenReplyFinished() {
    qDebug() << "O2FiwareOrion::onTokenReplyFinished";

    QNetworkReply *tokenReply = qobject_cast<QNetworkReply *>(sender());
    if (tokenReply->error() == QNetworkReply::NoError) {
        // Process reply
        QByteArray replyData = tokenReply->readAll();
        QVariantMap reply;
        foreach (QString pair, QString(replyData).split("&")) {
            QStringList kv = pair.split("=");
            if (kv.length() == 2) {
                reply.insert(kv[0], kv[1]);
            }
        }

        // Interpret reply
        this->setToken(reply.value(O2_OAUTH2_ACCESS_TOKEN, QString()).toString());
        this->setExpires(reply.value(OrionExpiresKey).toInt());
        this->setRefreshToken(reply.value(O2_OAUTH2_REFRESH_TOKEN, QString()).toString());
        this->setExtraTokens(reply);
        this->timedReplies_.remove(tokenReply);
        this->setLinked(true);
        Q_EMIT linkingSucceeded();
    } else {
        qWarning() << "O2FiwareOrion::onTokenReplyFinished:" << tokenReply->errorString();
    }
}

/// Handle failure of a token request.
void O2FiwareOrion::onTokenReplyError(QNetworkReply::NetworkError error){
    qWarning() << "O2FiwareOrion::onTokenReplyFinished:" << error;
}

/// Handle completion of a refresh request.
void O2FiwareOrion::onRefreshFinished(){
    qWarning() << "O2FiwareOrion::onRefreshFinished:";

}

/// Handle failure of a refresh request.
void O2FiwareOrion::onRefreshError(QNetworkReply::NetworkError error){
    qWarning() << "O2FiwareOrion::onRefreshError:" << error;
}
