#ifndef OSMAPIDRIVER_H
#define OSMAPIDRIVER_H

#include <QObject>
#include <QList>
#include <QMap>
#include "utils/api/SyncAPIDriver.h"
#include "utils/io/osm/OsmImporter.h"

class OsmAPIDriver : public QObject
{
    Q_OBJECT
public:
    explicit OsmAPIDriver( QObject *parent = 0);
    ~OsmAPIDriver();

    // STRUCTS
    struct OSMKeyValue {
        QString key;
        QString value;
    };

    // GETTERS

    /**
     * The download is done and parsed everytime this method is called to create new geometries
     * IMPORTANT CALLER GETS OWNERSHIP OF GEOMETRIES AND HAS TO DELETE THEM IF NOT IN USE
     * @param bounds
     * @param query
     * @return
     */
    QList<OsmImporter::OsmElement> getOsmElements( GWSEnvelope bounds , QList<OSMKeyValue> tags , bool nodes = true , bool ways = true , bool rels = true );

};

#endif // OSMAPIDRIVER_H
