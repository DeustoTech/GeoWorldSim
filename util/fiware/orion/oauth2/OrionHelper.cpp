#include "OrionHelper.h"

OrionHelper::OrionHelper(QObject *parent) : QObject(parent), oriondemo_(this), waitForMsg_(false), msg_(QString()) {

}

void OrionHelper::processArgs() {
    connect(&oriondemo_, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(&oriondemo_, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));

    // Start OAuth
    oriondemo_.doOAuth(O2::GrantFlowAuthorizationCode);
}

void OrionHelper::onLinkingFailed() {
    qDebug() << "Linking failed!";

}

void OrionHelper::onLinkingSucceeded() {
    qDebug() << "Linking succeeded!";
    if (waitForMsg_)
        qDebug() << "Waiting for message";
}
