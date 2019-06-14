#ifndef GWSDATASOURCEREADER_H
#define GWSDATASOURCEREADER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrent>

using namespace QtConcurrent;

class GWSDatasourceReader : public QObject
{
    Q_OBJECT

public:
    explicit GWSDatasourceReader( QString scenario_id , QString entities_type , QString entities_filter = "" , int limit = 99999999);

    // METHODS
    void startReading();
    QJsonArray getDownloadedData();
    bool downloadedFinished();

signals:
    void dataValueReadSignal( QJsonObject json_data );
    void dataReadingFinishedSignal();

private slots:
    void requestPaginated( int page );
    void dataReceived();

private:
    QString scenario_id;
    QString entities_type;
    QString entities_filter;
    unsigned int last_paginated = 0;
    unsigned int page_size = 1000;

    unsigned int downloaded_total = 0;
    unsigned int download_limit;

   // QJsonArray downloaded_data;
    bool downloaded_finished = false;

};

#endif // GWSDATASOURCEREADER_H
