#ifndef GWSDATASOURCEREADER_H
#define GWSDATASOURCEREADER_H

#include <QObject>
#include <QJsonObject>

#include "../../util/api/APIDriver.h"

class GWSDatasourceReader : public QObject
{
    Q_OBJECT

public:
    explicit GWSDatasourceReader( QString datasource_url , int limit = 9999999999);

    // METHODS
    void startReading();
    QJsonArray getDownloadedData();

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
    unsigned int page_size = 500;

    unsigned int downloaded_total = 0;
    unsigned int download_limit;

    QJsonArray downloaded_data;

};

#endif // GWSDATASOURCEREADER_H
