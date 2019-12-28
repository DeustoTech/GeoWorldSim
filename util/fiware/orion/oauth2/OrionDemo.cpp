#include "OrionDemo.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QMetaEnum>
#include <QDebug>

#include "o0globals.h"
#include "o0settingsstore.h"

const char ORION_APP_KEY[] = "bc49720ae7b94a8ca71d77c83982396e";
const char ORION_APP_SECRET[] = "a42b2ed6b2a4429cbb63d230612997ef";

const char ORION_REQUEST_URL[] = "https://account.lab.fiware.org/token";

const int localPort = 8888;

#define QENUM_NAME(o,e,v) (o::staticMetaObject.enumerator(o::staticMetaObject.indexOfEnumerator(#e)).valueToKey((v)))
#define GRANTFLOW_STR(v) QString(QENUM_NAME(O2, GrantFlow, v))

OrionDemo::OrionDemo(QObject *parent) : QObject(parent) {
    this->o2_fiware_orion = new O2FiwareOrion( this );
    this->o2_fiware_orion->setClientId(ORION_APP_KEY);
    this->o2_fiware_orion->setClientSecret(ORION_APP_SECRET);
    this->o2_fiware_orion->setLocalPort(localPort);
    this->o2_fiware_orion->setRequestUrl(ORION_REQUEST_URL);  // Use the desktop login UI
}

OrionDemo::~OrionDemo() {
    delete this->o2_fiware_orion;
}

void OrionDemo::doOAuth(O2::GrantFlow grantFlowType) {
    qDebug() << "Starting OAuth 2 with grant flow type" << GRANTFLOW_STR(grantFlowType) << "...";
    this->o2_fiware_orion->setGrantFlow(grantFlowType);

    connect(this->o2_fiware_orion, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(this->o2_fiware_orion, SIGNAL(linkingFailed()), this, SIGNAL(linkingFailed()));
    connect(this->o2_fiware_orion, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(this->o2_fiware_orion, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(this->o2_fiware_orion, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));

    this->o2_fiware_orion->unlink();
    this->o2_fiware_orion->link();

    // Create a store object for writing the received tokens
    O0SettingsStore *store = new O0SettingsStore(O2_ENCRYPTION_KEY);
    store->setGroupKey("orion");
    this->o2_fiware_orion->setStore(store);

;
}

void OrionDemo::validateToken() {
    if (!this->o2_fiware_orion->linked()) {
        qWarning() << "ERROR: Application is not linked!";
        emit linkingFailed();
        return;
    }

    QString accessToken = this->o2_fiware_orion->token();
    qDebug() << "Validating user token. Please wait...";
}

void OrionDemo::onOpenBrowser(const QUrl &url) {
    QDesktopServices::openUrl(url);
}

void OrionDemo::onCloseBrowser() {
    qDebug() << "Browser closed";
}


void OrionDemo::onLinkedChanged() {
    qDebug() << "Link changed!";
}

void OrionDemo::onLinkingSucceeded() {
    O2FiwareOrion *o1t = qobject_cast<O2FiwareOrion *>(sender());
    if (!o1t->linked()) {
        return;
    }
    QVariantMap extraTokens = o1t->extraTokens();
    if (!extraTokens.isEmpty()) {
        emit extraTokensReady(extraTokens);
        qDebug() << "Extra tokens in response:";
        foreach (QString key, extraTokens.keys()) {
            qDebug() << "\t" << key << ":" << (extraTokens.value(key).toString().left(3) + "...");
        }
    }
    emit linkingSucceeded();
}

void OrionDemo::onFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qWarning() << "NULL reply!";
        emit linkingFailed();
        return;
    }

    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Reply error:" << reply->error();
        qWarning() << "Reason:" << reply->errorString();
        emit linkingFailed();
        return;
    }

    QByteArray replyData = reply->readAll();
    bool valid = !replyData.contains("error");
    if (valid) {
        qDebug() << "Token is valid";
        emit linkingSucceeded();
    } else {
        qDebug() << "Token is invalid";
        emit linkingFailed();
    }
}
