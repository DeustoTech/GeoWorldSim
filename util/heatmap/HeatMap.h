#ifndef GSSHEATMAP_H
#define GSSHEATMAP_H

#include <QMap>

#include "utils/heatmap/HeatMapPalette.h"
#include "utils/geometry/Envelope.h"
#include "utils/geometry/Coordinate.h"

class GSSHeatMap
{

public:
    explicit GSSHeatMap();
    ~GSSHeatMap();

    // EXPORTERS
    virtual QJsonObject toJSON();
    virtual QImage toImage(const GWSEnvelope image_bounds, int image_width = 1024, int image_height = 1024) const;

    // SETTERS
    void addPoint( const GWSCoordinate &point , double weight = 1);

protected:
    QMap<GWSCoordinate , double> points;
    double max_value;

    // Color palette gradient
    GSSHeatMapPalette palette;

};

#endif // GSSHEATMAP_H
