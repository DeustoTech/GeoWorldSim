#ifndef ORIONHELPER_H
#define ORIONHELPER_H

#include <QObject>
#include "OrionDemo.h"

class OrionHelper : public QObject
{
    Q_OBJECT
public:
    explicit OrionHelper(QObject *parent = 0);

public slots:
    void processArgs();
    void onLinkingFailed();
    void onLinkingSucceeded();

private:
    OrionDemo oriondemo_;
    bool waitForMsg_;
    QString msg_;
};

#endif // ORIONHELPER_H
