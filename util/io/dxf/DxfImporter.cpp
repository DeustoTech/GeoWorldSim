#include "DxfImporter.h"
#include "utils/exporters/debugger/Debugger.h"

DxfImporter::DxfImporter() : DL_CreationAdapter (){
}

DxfImporter::~DxfImporter(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<DxfLayer> DxfImporter::getLayers(){
    return this->layers;
}

QList<DxfArc> DxfImporter::getArcs(){
    return this->arcs;
}

QList<DxfCircle> DxfImporter::getCircles(){
    return this->circles;
}

QList< DxfLine > DxfImporter::getLines(){
    return this->lines;
}

QList<DxfText> DxfImporter::getTexts(){
    return this->texts;
}

QList<DxfPoint> DxfImporter::getPoints(){
    return this->points;
}

QList< QList<DxfVertex> > DxfImporter::getPolylines(){
    return this->polylines;
}

/**********************************************************************
  METHODS
**********************************************************************/

void DxfImporter::closeFile(){
    if( !this->current_polyline.isEmpty() ){
        this->polylines.append( current_polyline );
    }
}

/**
 * Implementation of the method which handles layers.
 */
void DxfImporter::addLayer(const DL_LayerData& data) {
    qDebug("-- LAYER (%s, %6.3f)", data.name, data.flags);
    DxfLayer layer( QString::fromStdString( data.name ), data.flags );
    this->layers.append( layer );
}

/**
 * Implementation of the method which handles point entities.
 */
void DxfImporter::addPoint(const DL_PointData& data) {
    qDebug("-- POINT    (%6.3f, %6.3f, %6.3f)", data.x, data.y, data.z);
    DxfPoint point ( data.x, data.y, data.z );
    this->points.append( point );
}

/**
 * Implementation of the method which handles line entities.
 */
void DxfImporter::addLine(const DL_LineData& data) {
    qDebug("-- LINE     (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)", data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);
    DxfLine line( DxfPoint( data.x1, data.y1, data.z1 ), DxfPoint( data.x2, data.y2, data.z2 ) );
    this->lines.append( line );
}

/**
 * Implementation of the method which handles arc entities.
 */
void DxfImporter::addArc(const DL_ArcData& data) {
    qDebug("-- ARC      (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f", data.cx, data.cy, data.cz, data.radius, data.angle1, data.angle2);
    DxfArc arc( DxfPoint ( data.cx, data.cy, data.cz ), data.radius, data.angle1, data.angle2);
    this->arcs.append( arc );
}

/**
 * Implementation of the method which handles circle entities.
 */
void DxfImporter::addCircle(const DL_CircleData& data) {
    DxfCircle circle( DxfPoint( data.cx, data.cy, data.cz ), data.radius );
    this->circles.append( circle );
    qDebug("-- CIRCLE   (%6.3f, %6.3f, %6.3f) %6.3f", data.cx, data.cy, data.cz, data.radius);
}

/**
 * Implementation of the method which handles polyline entities.
 */
void DxfImporter::addPolyline(const DL_PolylineData& data) {
    if( !this->current_polyline.isEmpty() ){
        this->polylines.append( current_polyline );
    }
    this->current_polyline = QList<DxfVertex>();

    qDebug("********************************************************");
    qDebug("-- POLYLINE flags: %d  number of vertices: %d", (int)data.flags, (int)data.number);
}

/**
 * Implementation of the method which handles vertices.
 */
void DxfImporter::addVertex(const DL_VertexData& data) {
    DxfVertex vertex( DxfPoint( data.x, data.y, data.z ), data.bulge );
    qDebug("-- VERTEX   (%6.3f, %6.3f, %6.3f) %6.3f", data.x, data.y, data.z, data.bulge);

    if (data.x > 1 && data.y > 1 ){
        this->current_polyline.append( vertex );
    }
}

/**
 * Implementation of the method which handles texts.
 */
void DxfImporter::addText(const DL_TextData& data) {
    qDebug("-- TEXT     (%6.3f, %6.3f, %6.3f) %s", data.ipx, data.ipy, data.ipz, data.text.c_str());
    DxfText text( DxfPoint( data.ipx, data.ipy, data.ipz ), QString::fromStdString( data.text ) );
    this->texts.append( text );
}

/**
 * Implementation of the method which handles attributes.
 */
void DxfImporter::addAttribute(const DL_AttributeData& data){
    qDebug("-- ATTRIBUTE-TEXT     (%6.3f, %6.3f, %6.3f) %s", data.ipx, data.ipy, data.ipz, data.text.c_str());
    DxfText text( DxfPoint( data.ipx, data.ipy, data.ipz ), QString::fromStdString( data.text ) );
    this->texts.append( text );
}

/**
 * Retrieve text for specific coordinates.
 */
QString DxfImporter::getTextForCoordinates( GSSCoordinate coord ){
    QString text = "";
    foreach ( DxfText dx, this->texts ){

        if ( ( std::abs( coord.getX() - dx.point.x ) < 0.1 ) && ( std::abs( coord.getY() - dx.point.y ) < 0.1 ) ){
            text = dx.text;
            break;
        }
    }
    return text;
}
