#ifndef ORIONDEMO_H
#define ORIONDEMO_H

#include <QObject>

#include "O2FiwareOrion.h"

class OrionDemo : public QObject {
    Q_OBJECT

public:
    explicit OrionDemo(QObject *parent = 0);
    ~OrionDemo();

signals:
    void extraTokensReady(const QVariantMap &extraTokens);
    void linkingFailed();
    void linkingSucceeded();

public slots:
    void doOAuth(O2::GrantFlow grantFlowType);
    void validateToken();

private slots:
    void onLinkedChanged();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();
    void onFinished();

private:
    O2FiwareOrion* o2_fiware_orion;
};

#endif // ORIONDEMO_H
