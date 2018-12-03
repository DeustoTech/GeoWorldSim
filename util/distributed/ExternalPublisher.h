#ifndef EXTERNALPUBLISHER_H
#define EXTERNALPUBLISHER_H

#include <QJsonObject>

#include "ExternalCommunicator.h"

class GWSExternalPublisher : public GWSExternalCommunicator
{
Q_OBJECT

public:
    GWSExternalPublisher(QString socket_id);

    void sendMessage(QString signal , QJsonValue json);

};

#endif // EXTERNALPUBLISHER_H
