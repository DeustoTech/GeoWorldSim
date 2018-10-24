#ifndef GWSDATASOURCEREADER_H
#define GWSDATASOURCEREADER_H

#include <QObject>
#include <QJsonObject>

#include "../../util/api/APIDriver.h"

class GWSDatasourceReader : public QObject
{
    Q_OBJECT

public:
    explicit GWSDatasourceReader( QString datasource_url );

    // METHODS
    void startReading();

signals:
    void dataValueReadSignal( QJsonObject json_data );
    void dataReadingFinishedSignal();

private slots:
    void requestPaginated( int page );
    void dataReceived();

private:
    GWSAPIDriver api_driver;
    QString datasource_url;
    unsigned int last_paginated = 0;
    unsigned int page_size = 100;

    unsigned int downloaded_total = 0;
    unsigned int download_limit = 999999999;

};

#endif // GWSDATASOURCEREADER_H
