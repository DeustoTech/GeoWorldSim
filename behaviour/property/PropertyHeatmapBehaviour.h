#ifndef PROPERTYHEATMAPBEHAVIOUR_H
#define PROPERTYHEATMAPBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class PropertyHeatmapBehaviour : GWSBehaviour
{
    Q_OBJECT

public:


    // PARAMETERS
    static QString AGENTS_TYPE;
    static QString AGENTS_FILTER;
    static QString PROPERTY_TO_HEATMAP;
    static QString HEATMAP_DATA;
    static QString NEXTS;

     Q_INVOKABLE explicit PropertyHeatmapBehaviour();


private slots:
    QPair< double , QJsonArray > behave();


};

#endif // PROPERTYHEATMAPBEHAVIOUR_H
