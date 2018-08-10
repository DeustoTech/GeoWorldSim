#ifndef GWSSTYLE_H
#define GWSSTYLE_H

#include <QString>
#include <QColor>
#include <QJsonObject>

QT_FORWARD_DECLARE_CLASS(GWSAgent)

class GWSStyle
{

public:

    GWSStyle( GWSAgent* styled_agent );

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // PROPERTIES
    static QString COLOR_PROP;
    static QString OPACITY_PROP;
    static QString BORDER_COLOR_PROP;
    static QString BORDER_WEIGHT_PROP;
    static QString DASH_ARRAY_PROP;
    static QString ICON_URL_PROP;
    static QString ZOOM_LEVEL_PROP;

    // METHODS
    static QColor getRandomColor();
    static QString toHTML(QColor color);

public:

    // REF AGENT
    //GWSAgent* agent = Q_NULLPTR;

    QColor color;                           // Agent geometry inner color
    double opacity = 1;                     // Agent opacity [0,1]
    QColor border_color;                    // Agent geometry border color
    int border_weight = -1;                 // Agent geometry border weight
    QString dash_array;                     // Agent geometry line format
    QString icon_url;                       // Agent icon_url (only for points)
    double zoom_level = -1;                 // Zoom level from which on to appear
};

#endif // GWSSTYLE_H
