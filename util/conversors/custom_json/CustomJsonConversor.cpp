#include "CustomJsonConversor.h"

#include <QJsonValue>
#include <QJsonArray>

#include "agents/Agent.h"
#include "utils/grid/Grid.h"
#include "geos/geom/Envelope.h"

/**********************************************************************
 QVARIANT
**********************************************************************/

/**
 * @brief Creates the CUSTOM JSON format for attributes:
 *
 * properties {
 *  prop1 : {type : 'text' , value : 'Agent' , name : 'class'}
 *  prop2 : {...}
 * }
 *
 * @param value Property value
 * @return
 */
QJsonObject GWSCustomJsonConversor::formatVariant( QVariant value , QString units ){

    qRegisterMetaType<GWSUnit>();

    // Following https://schema.org/DataType
    QJsonObject json;

    if( !units.isEmpty() ){ json.insert("units" , units); }

    switch( value.type() ){
    case QVariant::String:
        json.insert("type" , QString("Text") );
        json.insert("value" , value.toString() );
        return json;
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Double:
    case QVariant::LongLong:
    case QVariant::ULongLong:
        json.insert("type" , QString("Number") );
        json.insert("value" , value.toDouble() );
        return json;
    case QVariant::Bool:
        json.insert("type" , QString("Boolean") );
        json.insert("value" , value.toBool() );
        return json;
    case QVariant::Color:
        json.insert("type" , QString("Color") );
        json.insert("value" , value.toString() );
        return json;
    case QVariant::DateTime:
    case QVariant::Date:
        json.insert("type" , QString("DateTime") );
        json.insert("value" , value.toDateTime().toString() );
        return json;
    case QVariant::Time:
        json.insert("type" , QString("Time") );
        json.insert("value" , value.toTime().toString() );
        return json;
    case QVariant::List: {
        QJsonArray values;
        foreach(QVariant v , value.toList()){
            QJsonObject json_value = GWSCustomJsonConversor::formatVariant( v );
            values.append( json_value.value("value") );
            json.insert("type" , json_value.value("type") );
        }
        json.insert("values" , values );
        return json;
    }
    default:

        // Check GWSUnit
        if( value.typeName() == QString("GWSUnit") ){ return GWSCustomJsonConversor::formatUnit( value.value<GWSUnit>() ); }

        // Check QJSonValue
        if( value.typeName() == "QJsonValue" ){ return GWSCustomJsonConversor::formatJsonValue( value.toJsonValue() ); }

        qWarning() << QString("Can not format unknown type %1 : %2").arg( value.typeName() ).arg( value.toString() );
    }
    return QJsonObject();
}

/**********************************************************************
 BASEUNIT
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatUnit(GWSUnit value){
    return GWSCustomJsonConversor::formatVariant( value.value , value.unit );
}

/**********************************************************************
 JSON VALUE
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatJsonValue(QJsonValue json){
    if( json.isBool() ){ return GWSCustomJsonConversor::formatVariant( json.toBool() ); }
    if( json.isDouble() ){ return GWSCustomJsonConversor::formatVariant( json.toDouble() ); }
    if( json.isObject() ){ return GWSCustomJsonConversor::formatVariant( json.toObject() ); }
    if( json.isString() ){ return GWSCustomJsonConversor::formatVariant( json.toString() ); }
}


/**********************************************************************
 JSON OBJECT
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatJsonObject(QJsonObject json){
    QJsonObject j;
    j.insert( "type" , "StructuredValue" );
    foreach(QString key , json.keys()){
        j.insert( key , GWSCustomJsonConversor::formatVariant( json.value( key ).toVariant() ));
    }
    return j;
}

/**********************************************************************
 GEOMETRY
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatGeometry(GWSGeometry* geom, QJsonObject properties ){
    QJsonObject json;
    json.insert("type" , QString("geo:json") );
    if( geom ){
        json.insert("value" , geom->toGeoJSON( properties ) );
    }
    return json;
}


/**********************************************************************
 AGENT
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatAgent(GWSAgent* agent ){
    QJsonObject json;
    json.insert("type" , QString("agent") );
    if( agent ){
        json.insert("value" , agent->getId() );
    }
    return json;
}

QJsonObject GWSCustomJsonConversor::formatStyle( GWSUiStyle* style ){
    QJsonObject json;
    if( !style ){ return json; }
    json.insert( "color", GWSCustomJsonConversor::formatVariant( style->color ) );
    json.insert( "fill_opacity", GWSCustomJsonConversor::formatVariant( style->color.alpha() / double( 255 ) ) ); // To be between 0 and 1
    json.insert( "border_color", GWSCustomJsonConversor::formatVariant( style->border_color ) );
    json.insert( "border_opacity", GWSCustomJsonConversor::formatVariant( style->border_color.alpha() / double( 255 ) ) ); // To be between 0 and 1
    json.insert( "weight", GWSCustomJsonConversor::formatVariant( style->border_weight ) );
    json.insert( "zoom_level", GWSCustomJsonConversor::formatVariant( style->zoom_level ) );
    if( !style->dash_array.isEmpty() ) { json.insert( "dash_array", GWSCustomJsonConversor::formatVariant( style->dash_array ) ); }
    if( !style->icon_url.isEmpty() ) { json.insert( "icon_url", GWSCustomJsonConversor::formatVariant( style->icon_url ) ); }
    return json;
}

/**********************************************************************
 GRID
**********************************************************************/


QJsonObject GWSCustomJsonConversor::formatGrid( GWSGrid grid ){
    QJsonObject json;
    json.insert("type" , QString("geo:json") );

    QJsonObject grid_json;
    grid_json["type"] = "FeatureCollection";

    QJsonArray features;

    for(unsigned int x = 0; x < grid.getXSize(); x++){
        for(unsigned int y = 0; y < grid.getYSize(); y++){

            GWSEnvelope envelope = grid.getCellEnvelope( x, y );
            GWSCoordinate envelope_centroid = envelope.getCentroid();

            double value = grid.getValue( envelope_centroid );
            QJsonObject properties;

            if( value != NAN ){
                properties.insert( "value" , value );
                properties.insert( "color" , GWSUiStyle::toHTML( QColor( 255 * ( 1 - ( ( grid.getMaxValue() - value ) / grid.getMaxValue() ) ) , 255 * ( ( grid.getMaxValue() - value ) / grid.getMaxValue() ) , 128  ) ) );
            }
        }
    }
    grid_json["features"] = features;
    json.insert("value" , grid_json );

    return json;
}


/**********************************************************************
 MAP
**********************************************************************/

QJsonObject GWSCustomJsonConversor::formatMap( QMap<QString , int> map , QString map_title){
    QMap<QString , double> dmap;
    foreach(QString k , map.keys()){
        dmap.insert( k , map.value(k) );
    }
    return GWSCustomJsonConversor::formatMap( dmap , map_title );
}

QJsonObject GWSCustomJsonConversor::formatMap( QMap<QString , double> map , QString map_title){

    // https://github.com/krispo/angular-nvd3/blob/gh-pages/js/discreteBarChart.js

    QJsonObject json;
    json.insert("type" , QString("barchart") );

    QJsonObject map_json;

    // DATA
    QJsonArray values;
    foreach( QString key , map.keys() ){
        QJsonObject value;
        value.insert( "key" , key );
        value.insert( "value" , map.value( key ) );
        values.append( value );
    }

    QJsonObject values_with_title;
    values_with_title.insert( "key" , map_title );
    values_with_title.insert( "values" , values );

    map_json.insert( "data" , QJsonArray( {{values_with_title}} ) );

    json.insert( "value" , map_json );

    return json;
}
