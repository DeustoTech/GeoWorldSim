#include "ImageDownloader.h"

#include "utils/api/SyncAPIDriver.h"

QMap<QUrl , QImage> ImageDownloader::downloaded_images = QMap<QUrl , QImage>();

QImage ImageDownloader::downloadImage(QUrl url){

    // See if it has been already downloaded
    if( ImageDownloader::downloaded_images.keys().contains( url ) ){
        return ImageDownloader::downloaded_images.value( url );
    }

    // Otherwise download from URL
    try {

        SyncAPIDriver* api = new SyncAPIDriver();
        QNetworkReply* reply = api->GET( url );

        if( reply->error() == QNetworkReply::NoError ){
            QImage image;
            image.loadFromData( reply->readAll() );
            if( !image.isNull() ){
                ImageDownloader::downloaded_images.insert( url , image );
            }
        }

        if( reply ){ reply->deleteLater(); }
        api->deleteLater();

    } catch (...){}

    return ImageDownloader::downloaded_images.value( url );
}
