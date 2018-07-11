#ifndef O2FIWAREORION_H
#define O2FIWAREORION_H

#include <QObject>

class O2FiwareOrion : public O2
{
    Q_OBJECT
public:
    explicit O2FiwareOrion(QObject *parent = 0);

protected slots:
    /// Handle verification response.
    virtual void onVerificationReceived(QMap<QString, QString>);

    /// Handle completion of a token request.
    virtual void onTokenReplyFinished();

    /// Handle failure of a token request.
    virtual void onTokenReplyError(QNetworkReply::NetworkError error);

    /// Handle completion of a refresh request.
    virtual void onRefreshFinished();

    /// Handle failure of a refresh request.
    virtual void onRefreshError(QNetworkReply::NetworkError error);
};

#endif // O2FIWAREORION_H
