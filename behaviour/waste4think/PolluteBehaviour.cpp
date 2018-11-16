#include "PolluteBehaviour.h"
#include <QFile>
#include "../../util/io/csv/CsvImporter.h"

QString PolluteBehaviour::POLLUTANT_TYPE = "pollutant_type";
QString PolluteBehaviour::STORE_POLLUTANT_VALUE_AS = "store_pollutant_value_as";
QString PolluteBehaviour::VEHICLE_TYPE = "vehicle_type";
QString PolluteBehaviour::VEHICLE_SUBTYPE = "vehicle_subtype";
QString PolluteBehaviour::NEXTS = "nexts";

PolluteBehaviour::PolluteBehaviour() : GWSBehaviour ()
{

}


QJsonArray PolluteBehaviour::behave(){


    QSharedPointer<GWSAgent> agent = this->getAgent();

    //QString vehicle_type = this->getProperty( VEHICLE_TYPE ).toString();
    QString vehicle_type = "Fuel";
    QString abatement = "abatement";
    QString vehicle_subtype = "Coaches Standard - Euro II - 91/542/EEC II";


    /*QString  vehicle_type = this->getProperty( VEHICLE_TYPE ).toString();
    QString vehicle_subtype = this->getProperty( VEHICLE_SUBTYPE ).toString();
*/

    QString path = "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/Waste4Think_V1/";
    QString filename = "RoadTransport_EEA_pollutants";
    QString extension = "csv";
    char separator = ',';


    CsvImporter* csv_importer = new CsvImporter( path , filename , extension , separator);
    QMap< QString, int > csv_headers = csv_importer->getColumnNames();

    QList< QString > csv_rows = csv_importer->getRows();
    qDebug() << csv_headers;

    foreach ( QString row , csv_rows ) {

        QMap< QString , QString > current_row = csv_importer->parseRow( row );
        qDebug() << current_row;
        qDebug() << current_row[ "ABATEMENT" ];
        if ( !current_row[ "ABATEMENT" ].isEmpty() && current_row[ "ABATEMENT" ]  == "Coaches Standard - Euro II - 91/542/EEC II" ){
            qDebug() << "Found the vehicle type you were searching for!" << current_row;
        }

    }



    /*QFile file("/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/Waste4Think_V1/RoadTransport_EEA_pollutants.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return QStringList();
    }

   QTextStream stream(&file);
   QStringList vehicles;
   QStringList pollutant_type;
   QStringList pollutant_value;
   while (!stream.atEnd())
   {
      QString line = stream.readLine();
      QStringList fields = line.split( ',' );
      vehicles.append( fields[ 4 ] );
      qDebug() << "Column header" << fields[ 4 ];
      qDebug() << "Column data" << vehicles;
   }
   file.close();*/


   return this->getProperty( NEXTS ).toArray();

}
