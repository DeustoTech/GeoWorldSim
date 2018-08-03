#include "Style.h"

#include <QStringList>
#include <QJsonArray>

QString GWSStyle::COLOR_PROP = "color";
QString GWSStyle::BORDER_COLOR_PROP = "border_color";
QString GWSStyle::BORDER_WEIGHT_PROP = "border_weight";
QString GWSStyle::DASH_ARRAY_PROP = "dash_array";
QString GWSStyle::ICON_URL_PROP = "icon_url";
QString GWSStyle::ZOOM_LEVEL_PROP = "zoom_level";

GWSStyle::GWSStyle(GWSObject *parent) : GWSObject( parent ){
}

GWSStyle::~GWSStyle(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QColor GWSStyle::getRandomColor(){
    int rand = qrand() % QColor::colorNames().size();
    return QColor( QColor::colorNames().at( rand ) );
}

QString GWSStyle::toHTML(QColor color){
    return color.name();
}
