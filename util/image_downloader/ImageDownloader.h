#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QImage>

class ImageDownloader : public QObject
{
    Q_OBJECT

public:

    static QImage downloadImage( QUrl );

private:

    static QMap<QUrl , QImage> downloaded_images;

};

#endif // IMAGEDOWNLOADER_H
