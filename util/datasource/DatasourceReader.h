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

signals:
    void dataValueReadSignal( QJsonObject json_data );

private slots:
    void requestPaginated( int page );
    void dataReceived();

private:
    GWSAPIDriver api_driver;
    QString datasource_url;
    unsigned int last_paginated = 0;
    unsigned int limit = 100;

};

#endif // GWSDATASOURCEREADER_H
