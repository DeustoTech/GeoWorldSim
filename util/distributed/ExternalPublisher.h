#ifndef GWSEXTERNALPUBLISHER_H
#define GWSEXTERNALPUBLISHER_H

#include <QJsonObject>

#include "ExternalCommunicator.h"

class GWSExternalPublisher : public GWSExternalCommunicator
{
Q_OBJECT

public:
    GWSExternalPublisher(const QString &socket_id);

    void sendMessage(const QString &signal , const QJsonValue &json);

};

#endif // GWSEXTERNALPUBLISHER_H
