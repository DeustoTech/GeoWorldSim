#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QMutex>

#include "agents/Agent.h"

#include <marble/GeoDataDocument.h>
#include <marble/MarbleWidget.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataFeature.h>
#include <marble/GeoDataGeometry.h>
#include <marble/GeoDataStyle.h>
#include "environment/Environment.h"

using namespace geos::geom;
using namespace Marble;

class MapWidget : public MarbleWidget
{
    Q_OBJECT

public:
    MapWidget(Environment* environment, QObject* parent = 0 );
    ~MapWidget();

    // Methods
    void showMap();
    GeoDataFeature* agentToFeature(Agent* agent);
    GeoDataGeometry* geometryToGeoDataGeometry(geos::geom::Geometry* geometry);
    GeoDataStyle::Ptr styleToGeoDataStyle(UiStyle* style);   

private:

    QMap<QString, GeoDataDocument*> map_documents;
    QMap<QString, QMap<unsigned int, GeoDataFeature*> > map_features; // QMAP<ClassName, QMAP< ID , AGENT>>
    GeoDataDocument* root;

    // Mutex, for avoiding concurrency
    QMutex mutex;

    Environment* environment;

    // Private methods
    void addAgent(Agent* agent);
    void addAgents(QList<Agent*> agents);
    void updateAgent(Agent* agent);
    void updateAgents(QList<Agent*> agents);
    void removeAgent(Agent* agent);
    void removeAgents(QList<Agent*> agents);
};

#endif // MAPWIDGET_H
