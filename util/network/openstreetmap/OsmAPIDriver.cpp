#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

#include "OsmAPIDriver.h"

OsmAPIDriver::OsmAPIDriver(QObject *parent) : QObject(parent){
}

OsmAPIDriver::~OsmAPIDriver(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/**
  WARNING! Caller takes responsability to delete geometries it doesnt want to use
 * @brief OsmAPIDriver::downloadFromOsm
 * @param bounds
 * @param tags
 * @param nodes
 * @param ways
 * @param relations
 * @return
 */
QList<OsmImporter::OsmElement> OsmAPIDriver::getOsmElements( GWSEnvelope bounds, QList<OSMKeyValue> tags,  bool nodes, bool ways, bool relations){

    //http://wiki.openstreetmap.org/wiki/Overpass_API/Language_Guide

    QString bbox = QString("[bbox:%1,%2,%3,%4]").arg( bounds.getMinY() ).arg( bounds.getMinX() ).arg( bounds.getMaxY() ).arg( bounds.getMaxX() );

    QString query = bbox+"[out:xml][timeout:2500]; (";

    // NODES
    if( nodes ){
        query += "node";

        foreach(OSMKeyValue tag , tags){
            query += "[\"" + tag.key + "\"";
            if( !tag.value.isEmpty() ){
                query += "=\"" + tag.value + "\"";
            }
            query += "]";
        }
    }

    // WAYS
    if( ways ){
        query += "way";


        foreach(OSMKeyValue tag , tags){
            query += "[\"" + tag.key + "\"";
            if( !tag.value.isEmpty() ){
                query += "=\"" + tag.value + "\"";
            }
            query += "]";
        }
    }

    // RELATIONS
    if( relations ){
        query += "relation";

        foreach(OSMKeyValue tag , tags){
            query += "[\"" + tag.key + "\"";
            if( !tag.value.isEmpty() ){
                query += "=\"" + tag.value + "\"";
            }
            query += "]";
        }
    }

    query += ");out body; >; out skel qt;";

    QUrlQuery qurl_query;
    qurl_query.addQueryItem("data" , query);

    QMap<QString, QString> headers;
    headers.insert( "application" , "json" );

    QList<OsmImporter::OsmElement> elements;

    SyncAPIDriver* api = new SyncAPIDriver( this );
    QNetworkReply* reply = api->POST( QUrl("https://overpass-api.de/api/interpreter") , headers , qurl_query.toString(QUrl::FullyEncoded).toUtf8() );
    api->deleteLater();

    // If no reply
    if( !reply ){
        return elements;
    }

    // If the request returned an error
    if( reply->error() != QNetworkReply::NoError ){
        qWarning() << Q_FUNC_INFO << "Network error" << reply->errorString() << endl;
        reply->deleteLater();
        return elements;
    }

    // Else create OSM temporary file and parse it
    QFile temporary_file( QDir::currentPath() + "/" + "osm_api_temp.osm");
    if( temporary_file.exists() ){
        temporary_file.remove();
    }

    if( temporary_file.open( QIODevice::WriteOnly ) ){

        temporary_file.write( reply->readAll() );
        temporary_file.close();

        OsmImporter* importer = new OsmImporter( QDir::currentPath() , "osm_api_temp" );

        foreach(OsmImporter::OsmElement e , importer->getOsmElements( nodes , ways , relations ) ){
            elements.append( e );
        }

        importer->deleteLater();
        QFile::remove( temporary_file.fileName() );
    }

    reply->deleteLater();

    return elements;
}
