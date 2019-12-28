#ifndef VIDEOEXPORTER_H
#define VIDEOEXPORTER_H

#include <QObject>
#include <QVideoFrame>

#include "utils/geometry/GSSEnvelope.h"
#include "agents/Agent.h"
#include "environment/GSSEnvironment.h"

class VideoExporter : public QObject
{
    Q_OBJECT
public:
    explicit VideoExporter(GSSEnvironment* environment , GSSEnvelope envelope , QObject *parent = 0);

    // METHODS
    void start();
    void stop();

public slots:

    void createFrame(QString class_name , QString id);

protected:

    QVideoFrame generateFrame( int step = 0 );

private:

    GSSEnvironment* environment;
    GSSEnvelope envelope;
    QStringList export_agent_types = { "GTPLATAgent" , "PublicTransportVehicle" };
    quint64 last_rendered_datetime;
};

#endif // VIDEOEXPORTER_H
