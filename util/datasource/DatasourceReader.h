#ifndef GWSDATASOURCEREADER_H
#define GWSDATASOURCEREADER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class GWSDatasourceReader : public QObject
{
    Q_OBJECT

public:
    explicit GWSDatasourceReader( QString scenario_id , QString entities_type , int limit = 9999999999 , QString min_time = "" , QString max_time = "" );

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
    QString min_time;
    QString max_time;
    unsigned int last_paginated = 0;
    unsigned int page_size = 1000;

    unsigned int downloaded_total = 0;
    unsigned int download_limit;

    QJsonArray downloaded_data;
    bool downloaded_finished = false;

};

#endif // GWSDATASOURCEREADER_H
