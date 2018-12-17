#ifndef GWSEXTERNALLISTENER_H
#define GWSEXTERNALLISTENER_H

#include "ExternalCommunicator.h"

#include <QJsonObject>

class GWSExternalListener : public GWSExternalCommunicator
{
    Q_OBJECT

public:
    GWSExternalListener( QString socket_id );

signals:
    void dataReceivedSignal( QJsonObject json_data );

public slots:

};

#endif // GWSEXTERNALLISTENER_H
