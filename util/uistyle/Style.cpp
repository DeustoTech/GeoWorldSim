#include "Style.h"

#include <QStringList>
#include <QJsonArray>
#include <QColor>

#include "../../agent/Agent.h"

QString GWSStyle::COLOR_PROP = "color";
QString GWSStyle::BORDER_COLOR_PROP = "border_color";
QString GWSStyle::BORDER_WEIGHT_PROP = "border_weight";
QString GWSStyle::DASH_ARRAY_PROP = "dash_array";
QString GWSStyle::ICON_URL_PROP = "icon_url";
QString GWSStyle::ZOOM_LEVEL_PROP = "zoom_level";

GWSStyle::GWSStyle(GWSAgent *styled_agent){
    Q_UNUSED( styled_agent );
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSStyle::deserialize(QJsonObject json){
    if( json.keys().contains( COLOR_PROP ) ){           this->color = json.value( COLOR_PROP ).toString(); }
    if( json.keys().contains( BORDER_COLOR_PROP ) ){    this->color = json.value( BORDER_COLOR_PROP ).toString(); }
    if( json.keys().contains( BORDER_WEIGHT_PROP ) ){   this->border_weight = json.value( BORDER_WEIGHT_PROP ).toInt(); }
    if( json.keys().contains( DASH_ARRAY_PROP ) ){      this->dash_array = json.value( DASH_ARRAY_PROP ).toString(); }
    if( json.keys().contains( ICON_URL_PROP ) ){        this->icon_url = json.value( ICON_URL_PROP ).toString(); }
    if( json.keys().contains( ZOOM_LEVEL_PROP ) ){      this->zoom_level = json.value( ZOOM_LEVEL_PROP ).toInt(); }
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSStyle::serialize() const {
    QJsonObject json;
    if( this->color.isValid() ){        json.insert( COLOR_PROP ,                   GWSStyle::toHTML( this->color ) ); }
    if( this->border_color.isValid() ){ json.insert( BORDER_COLOR_PROP ,            GWSStyle::toHTML( this->border_color ) ); }
    if( this->border_weight >= 0 ){     json.insert( BORDER_WEIGHT_PROP ,           this->border_weight ); }
    if( !this->dash_array.isEmpty() ){  json.insert( DASH_ARRAY_PROP ,              this->dash_array ); }
    if( !this->icon_url.isEmpty() ){    json.insert( ICON_URL_PROP ,                this->icon_url );  }
    if( this->zoom_level >= 0 ){        json.insert( ZOOM_LEVEL_PROP ,              this->zoom_level ); }
    return json;
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
