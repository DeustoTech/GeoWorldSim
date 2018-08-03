#ifndef GWSSTYLE_H
#define GWSSTYLE_H

#include "../../object/Object.h"
#include <QJsonObject>
#include <QVariant>
#include <QImage>
#include <QColor>

class GWSStyle : public GWSObject
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GWSStyle( GWSObject* parent );
    ~GWSStyle();

    // PROPERTIES
    static QString COLOR_PROP;
    static QString BORDER_COLOR_PROP;
    static QString BORDER_WEIGHT_PROP;
    static QString DASH_ARRAY_PROP;
    static QString ICON_URL_PROP;
    static QString ZOOM_LEVEL_PROP;

    // METHODS
    static QColor getRandomColor();
    static QString toHTML(QColor color);

    // VARIABLES
    //QColor color = QColor("Gray");          // Agent geometry inner color
    //QColor border_color = QColor("Black");  // Agent geometry border color
    //int border_weight = 5;                  // Agent geometry border weight
    //QString dash_array;                     // Agent geometry line format
    //QString icon_url;                       // Agent icon_url (only for points)
    //QImage icon;                            // Agent image
    //double zoom_level = 0;                  // Zoom level from which on to appear
};

Q_DECLARE_METATYPE(GWSStyle*)

#endif // GWSSTYLE_H
