#include "InpExporter.h"

#include <QSqlRecord>
#include <QVariant>
#include <QDebug>

#include "QTextStream"
#include "utils/db/PostgresDriver.h"
#include "usecases/estolzain/settings/EstolZainSettings.h"

InpExporter::InpExporter(){
}

/**********************************************************************
 METHODS
**********************************************************************/

// Export to INP
bool InpExporter::process( QString filepath ) {

    // Get INP template File
    QString import_path = EstolZainSettings::getInstance()->getSetting( "INP_TEMPLATE_PATH" ).toString();
    if ( !filepath.endsWith( EstolZainSettings::getInstance()->getSetting( "INP_FILE_EXTENSION" ).toString() ) ){
        filepath = filepath + EstolZainSettings::getInstance()->getSetting( "INP_FILE_EXTENSION" ).toString();
    }
    QString export_path = filepath;

    qDebug() << "[InpExporter::process] Retrieving INP template file in path " << import_path;

    this->input_inp.setFileName( import_path );
    if (!input_inp.open(QIODevice::ReadOnly | QIODevice::Text) || !input_inp.exists()){
        qDebug() << "[InpExporter::process] INP template file does not exist in path " <<  import_path;
        return false;
    }

    this->output_inp.setFileName( export_path );
    if ( !output_inp.open(QIODevice::WriteOnly | QIODevice::Text) ){
        qDebug() << "[InpExporter::process] Can not write INP export file to path " << import_path;
        return false;
    }

    QString sql = QString( "SELECT target.id as target_id, target.name as target_name, lines, main.id as main_id, main.dbase_table as table_name \
                           FROM inp_target as target \
                           INNER JOIN inp_table as main ON target.table_id = main.id" );

    PostgresDriver* driver = new PostgresDriver ( "energia.deusto.es", 5432, "estolzain", "postgres", "menos pantalla y mas estadistica" );

    QListIterator<QSqlRecord> j( driver->executeQuery(sql) );
    while( j.hasNext() ){
        QSqlRecord record = j.next();
        this->processTarget( record.value("target_id").toInt(), record.value("table_name").toString(), record.value("lines").toInt() );
    }
    delete driver;
}

bool InpExporter::processTarget(int id, QString table_name, int lines){

    QTextStream out(&this->output_inp);
    bool write_coordinates = false;
    bool write_junctions = false;
    bool write_conduits = false;
    bool write_vertices = false;

    // Go to the first line of the target
    for (int i = 1; i <= lines; i++) {
        QByteArray line = this->input_inp.readLine();
        out << line;

        if ( line.contains("[COORDINATES]") ){
            write_coordinates = true;
        }
        else if ( line.contains("[JUNCTION]") ){
            write_junctions = true;
        }
        else if ( line.contains("[CONDUITS]") ){
            write_conduits = true;
        }
        else if ( line.contains("[VERTICES]") ){
            write_vertices = true;
        }
    }

    if ( write_coordinates ){
        foreach( InpCoordinate inp_coordinate, this->coordinates){
            out << inp_coordinate.node_id << " "  << inp_coordinate.x << " " << inp_coordinate.y << "\n";
        }
    }
    else if ( write_junctions ){
        foreach( InpJunction inp_junction, this->junctions){
            out << inp_junction.node_id << " "  << inp_junction.elev << " " << inp_junction.ymax << " "
                << inp_junction.y0      << " "  << inp_junction.ysur << " " << inp_junction.apond << "\n";
        }
    }
    else if ( write_conduits ){
        foreach( InpConduit inp_conduit, this->conduits){
            out << inp_conduit.arc_id << " "  << inp_conduit.n1     << " "   << inp_conduit.n2 << " "
                << inp_conduit.length << " "  << inp_conduit.nvalue << " "   << inp_conduit.zup << " "
                << inp_conduit.zdown  << " "  << inp_conduit.q0     << inp_conduit.qmax << "\n";
        }
    }
    else if ( write_vertices ){
        foreach( InpVertex inp_vertex, this->vertices){
            out << inp_vertex.arc_id << " " << inp_vertex.x << " " << inp_vertex.y << "\n";
        }
    }
}

bool InpExporter::processTarget2(int id, QString table_name, int lines){
    return false;
}

void InpExporter::parseField(QMap<QString, QString> row_data, QString skey, int size){

}

QList< QMap<QString, QString> > InpExporter::getTableData(QString table_name){

}

void InpExporter::addConduit(InpConduit conduit){
    this->conduits.append( conduit );

}
void InpExporter::addVertex(InpVertex vertex){
    this->vertices.append( vertex );
}

void InpExporter::addJunction(InpJunction junction){
    this->junctions.append( junction );
}

void InpExporter::addCoordinate(InpCoordinate coordinate){
    this->coordinates.append( coordinate );
}

