#include "Style.h"

#include <QStringList>
#include <QJsonArray>

GWSStyle::GWSStyle(GWSObject *parent) : GWSObject( parent ){
}

GWSStyle::~GWSStyle(){
}

void GWSStyle::deserialize( QJsonObject json ){
    if( json.contains("color") ) { this->color = QColor( json["color"].toObject()["value"].toString() ); }
    if( json.contains("fill_opacity") ) { this->color.setAlpha( json["fill_opacity"].toObject()["value"].toString().toDouble() * 255 ); } // To be between 0 and 255
    if( json.contains("border_color") ) { this->border_color = QColor( json["border_color"].toObject()["value"].toString() ); }
    if( json.contains("border_opacity") ) { this->border_color.setAlpha( json["border_opacity"].toObject()["value"].toString().toDouble() * 255 ); } // To be between 0 and 255
    if( json.contains("weight") ) { this->border_weight = json["weight"].toObject()["value"].toString().toInt(); }
    if( json.contains("icon_url") ) { this->icon_url = json["icon_url"].toObject()["value"].toString(); }
    if( json.contains("dash_array") ) { this->dash_array = json["dash_array"].toObject()["value"].toString(); }
    if( json.contains("zoom_level") ) { this->zoom_level = json["zoom_level"].toObject()["value"].toInt(); }
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
