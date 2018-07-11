#ifndef INEMICRODATAIMPORTER_H
#define INEMICRODATAIMPORTER_H

#include <QObject>
#include <QList>

#include "utils/io/ImporterExporter.h"


class IneMicroDataImporter : public GSSImporterExporter
{
    Q_OBJECT
public:

    struct Record {
        // Person info
        QString province;
        QString municipality;
        QString born_month;
        QString born_year;
        QString age;
        QString gender;

        // Living
        QString living_country;
        QString living_province;
        QString living_municipality;

        // Work
        QString working_place;
        QString working_country;
        QString working_province;
        QString working_municipality;
        QString working_postcode;
        QString working_travels_amount;
        QString working_transport_type_1;
        QString working_transport_type_2;
        QString working_travel_time;

    };

    explicit IneMicroDataImporter(QString directory_path , QString filename , QObject *parent = 0);

    // GETTERS
    QList<IneMicroDataImporter::Record> getRows();

private:
    IneMicroDataImporter::Record processLine(QString line);

};

#endif // INEMICRODATAIMPORTER_H
