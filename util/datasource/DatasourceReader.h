#ifndef GWSDATASOURCEREADER_H
#define GWSDATASOURCEREADER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class GWSDatasourceReader : public QObject
{
    Q_OBJECT

public:
    explicit GWSDatasourceReader( QString user_id , QString scenario_id , QString entity_type , QString entity_filter = "" , int limit = 99999999);

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
    QString user_id;
    QString entity_type;
    QString entity_filter;
    unsigned int last_paginated = 0;
    unsigned int page_size = 1000;

    unsigned int downloaded_total = 0;
    unsigned int download_limit;

   // QJsonArray downloaded_data;
    bool downloaded_finished = false;

};

#endif // GWSDATASOURCEREADER_H
