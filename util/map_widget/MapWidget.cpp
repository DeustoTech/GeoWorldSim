#include "MapWidget.h"

#include <QThread>
#include <QMutexLocker>
#include <QTreeView>

#include "geos/geom/MultiPoint.h"
#include "geos/geom/MultiLineString.h"
#include "geos/geom/MultiPolygon.h"

#include <marble/MarbleModel.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/GeoDataPlacemark.h>

#include <marble/GeoDataStyle.h>
#include <marble/GeoDataBalloonStyle.h>
#include <marble/GeoDataLineStyle.h>
#include <marble/GeoDataPolyStyle.h>

#include <marble/GeoDataPoint.h>
#include <marble/GeoDataLineString.h>
#include <marble/MarbleWidgetPopupMenu.h>
#include <marble/MarbleWidgetInputHandler.h>
#include <QObject>


MapWidget::MapWidget(Environment* environment, QObject* parent) : MarbleWidget(){
    Q_UNUSED(parent);

    this->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");

    this->environment = environment;

    // Connect to Environment signals
    QObject::connect( this->environment , SIGNAL(interfaceSignal(Agent*,QString)) , this , SLOT(pushAgent(Agent*,QString)) );
    QObject::connect( this->environment , SIGNAL(interfaceSignal(QList<Agent*>,QString)) , this , SLOT(pushAgents(QList<Agent*>,QString)) );
}

MapWidget::~MapWidget(){    
    //Disconnect from Environment signals
    QObject::disconnect( this->environment , SIGNAL(interfaceSignal(Agent*,QString)) , this , SLOT(pushAgent(Agent*,QString)) );
    QObject::disconnect( this->environment , SIGNAL(interfaceSignal(QList<Agent*>,QString)) , this , SLOT(pushAgents(QList<Agent*>,QString)) );
}


/**********************************************************************
 SETTERS
**********************************************************************/

void MapWidget::addAgent(Agent *agent){

    if( this->map_features.value( agent->metaObject()->className() ).keys().contains( agent->getId() ) ){
        qWarning() << "[MapWidget::addAgent] Agent" << agent->metaObject()->className() << agent->getId() << "already in map_widget." << endl;
        return;
    }

    {
        QMutexLocker locker(&this->mutex);

        // If it is the first agent of this class we receive, create the class in the map
        if( !this->map_features.keys().contains( agent->metaObject()->className() ) ){

            GeoDataDocument* document = new GeoDataDocument();
            this->map_documents.insert( agent->metaObject()->className() , document );
            this->model()->treeModel()->addDocument( document );

            // Insert map with the agents class
            QMap<unsigned int , GeoDataFeature*> map;
            this->map_features.insert( agent->metaObject()->className() , map );
        }
    }

    // Add to maps
    GeoDataFeature* feature = this->agentToFeature( agent );

    if( feature ){
        this->map_features[ agent->metaObject()->className() ].insert( agent->getId() , feature );
        GeoDataDocument* document = this->map_documents.value( agent->metaObject()->className() , 0 );
        if( document ){
            {
                QMutexLocker locker(&this->mutex);
                this->model()->treeModel()->addFeature( dynamic_cast<GeoDataDocument*>( document ) , feature );
            }
        }
    }
}


void MapWidget::addAgents(QList<Agent*> agents){
    foreach(Agent* a , agents){
        this->addAgent( a );
    }
}


void MapWidget::updateAgent(Agent* agent){
    if( this->map_documents.keys().contains( agent->metaObject()->className() ) ){
        GeoDataFeature* feature = this->map_features.value( agent->metaObject()->className() ).value( agent->getId() , 0 );
        if( feature ){
            const GSSGeometry* agent_geom = agent->getGeometry();
            if( agent_geom ){
                //dynamic_cast<GeoDataPlacemark*>( feature )->setGeometry( this->geometryToGeoDataGeometry( agent_geom ) );
                delete agent_geom;
            }
            GeoDataStyle::Ptr style = this->styleToGeoDataStyle( agent->getStyle() );
            if( style ) { feature->setStyle( style ); }
            {
                QMutexLocker locker(&this->mutex);
                try {
                    this->model()->treeModel()->updateFeature( feature );
                } catch(...){
                    qWarning() << "[MapWidget::updateAgent] Crashed updating agent" << endl;
                }
            }
        }
    }
}


void MapWidget::updateAgents(QList<Agent*> agents){
    foreach(Agent* a , agents){
        this->updateAgent( a );
    }
}


void MapWidget::removeAgent(Agent *agent){

    // Remove from maps
    if( this->map_documents.keys().contains( agent->metaObject()->className() ) ){
        GeoDataFeature* feature = this->map_features[ agent->metaObject()->className() ].take( agent->getId() );
        if( feature ){
            {
                QMutexLocker locker(&this->mutex);
                this->model()->treeModel()->removeFeature( feature );
            }
            delete feature;
        }
    }
}


void MapWidget::removeAgents(QList<Agent*> agents){
    foreach(Agent* a , agents){
       this->removeAgent( a );
    }
}

/**********************************************************************
 METHODS
**********************************************************************/

void MapWidget::showMap(){
    MarbleWidget::show();
}

GeoDataStyle::Ptr MapWidget::styleToGeoDataStyle(UiStyle* agent_style){

    if( !agent_style ){
        qWarning() << "[MapWidget::styleToGeoDataStyle] Tried parse empty style." << endl;
//        return 0;
    }

    // Style
    GeoDataStyle::Ptr style(new GeoDataStyle);
    // Baloon Style
    GeoDataBalloonStyle bs = GeoDataBalloonStyle();
    bs.setColor( agent_style->color );
    bs.setBackgroundColor( agent_style->color );
    bs.setDisplayMode( GeoDataBalloonStyle::Default );
    bs.setTextColor( agent_style->color );
    style->setBalloonStyle( bs );   
    // Line Style
    GeoDataLineStyle ls = GeoDataLineStyle( agent_style->border_color );
    ls.setColor( agent_style->border_color );
    ls.setWidth( agent_style->border_weight );
    style->setLineStyle( ls );
    // Poly Style
    GeoDataPolyStyle ps = GeoDataPolyStyle( agent_style->color );
    ps.setColor( agent_style->color );
    ps.setFill( true );
    style->setPolyStyle( ps );
    // Label Style
    GeoDataLabelStyle s = GeoDataLabelStyle();
    s.setScale(20);
    style->setLabelStyle( s );

    return style;
}

GeoDataGeometry* MapWidget::geometryToGeoDataGeometry(geos::geom::Geometry* agent_geometry){

    if( !agent_geometry || !agent_geometry->isValid() ){
        qWarning() << "[MapWidget::geometryToGeoDataGeometry] Tried parse empty geometry." << endl;
        return 0;
    }

    geos::geom::Geometry* geom = agent_geometry->clone();
    GeoDataGeometry* geometry = 0;

    try {

        // POINT
        if( geom->getGeometryTypeId() == geos::geom::GEOS_POINT ){

            geos::geom::Point* point = dynamic_cast<geos::geom::Point*>(geom);
            if( point ){
                qreal lon = point->getCoordinate()->x;
                qreal lat = point->getCoordinate()->y;
                qreal alt = point->getDimension() > 2 ? point->getCoordinate()->z : 0;
                geometry = new GeoDataPoint( lon , lat , alt , GeoDataCoordinates::Degree );
            }

        // LINESTRING
        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_LINESTRING ){

            geos::geom::LineString* line = dynamic_cast<geos::geom::LineString*>(geom);

            if( line ){
                geometry = new GeoDataLineString();

                for(unsigned int i = 0; i < line->getCoordinates()->size(); i++){                    
                    qreal lon = line->getCoordinates()->getAt(i).x;
                    qreal lat = line->getCoordinates()->getAt(i).y;
                    qreal alt = line->getDimension() > 2 ? line->getCoordinates()->getAt(i).z : 0;
                    dynamic_cast<GeoDataLineString*>( geometry )->append( GeoDataCoordinates( lon , lat , alt , GeoDataCoordinates::Degree ) );
                }
            }

        // POLYGON
        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON ) {

            geos::geom::Polygon* polygon = dynamic_cast<geos::geom::Polygon*>(geom);

            if( polygon ){
                geometry = new GeoDataPolygon();
                GeoDataLinearRing r;

                const LineString* ext = polygon->getExteriorRing();
                for(unsigned int i = 0; i < ext->getCoordinates()->size(); i++){
                    qreal lon = ext->getCoordinates()->getAt(i).x;
                    qreal lat = ext->getCoordinates()->getAt(i).y;
                    qreal alt = ext->getCoordinates()->getDimension() > 2 ? ext->getCoordinates()->getAt(i).z : 0;
                    r.append( GeoDataCoordinates( lon , lat , alt , GeoDataCoordinates::Degree ) );
                }
                dynamic_cast<GeoDataPolygon*>( geometry )->setOuterBoundary( r );
            }

        // MULTIPOINT
        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOINT ) {

            geos::geom::MultiPoint* multi = dynamic_cast<geos::geom::MultiPoint*>(geom);

            if( multi ){
                geometry = new GeoDataMultiGeometry();
                for(unsigned int i = 0; i < multi->getNumGeometries(); i++){
                    geos::geom::Geometry* g = multi->getGeometryN(i)->clone();
                    GeoDataGeometry* gg = this->geometryToGeoDataGeometry( g );
                    if( gg ){ dynamic_cast<GeoDataMultiGeometry*>( geometry )->append( gg ); }
                    delete g;
                }
            }

        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_MULTILINESTRING ) {

            geos::geom::MultiLineString* multi = dynamic_cast<geos::geom::MultiLineString*>(geom);

            if( multi ){
                geometry = new GeoDataMultiGeometry();
                for(unsigned int i = 0; i < multi->getNumGeometries(); i++){
                    geos::geom::Geometry* g = multi->getGeometryN(i)->clone();
                    GeoDataGeometry* gg = this->geometryToGeoDataGeometry( g );
                    if( gg ){ dynamic_cast<GeoDataMultiGeometry*>( geometry )->append( gg ); }
                    delete g;
                }
            }

        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON ) {

            geos::geom::MultiPolygon* multi = dynamic_cast<geos::geom::MultiPolygon*>(geom);

            if( multi ){
                geometry = new GeoDataMultiGeometry();
                for(unsigned int i = 0; i < multi->getNumGeometries(); i++){
                    geos::geom::Geometry* g = multi->getGeometryN(i)->clone();
                    GeoDataGeometry* gg = this->geometryToGeoDataGeometry( g );
                    if( gg ){ dynamic_cast<GeoDataMultiGeometry*>( geometry )->append( gg ); }
                    delete g;
                }
            }

        // GEOMETRYCOLLECTION
        } else if( geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION ) {


        } else {
            qWarning() << "[MapWidget::geometryToGeoDataGeometry] Geometry type" << QString::fromStdString( geom->getGeometryType() ) << "unknown" << endl;
        }

    } catch(...){
        qCritical() << "[MapWidget::geometryToGeoDataGeometry] Error parsing agent geometry" << endl;
    }

    delete geom;
    return geometry;
}


GeoDataFeature* MapWidget::agentToFeature(Agent *agent){

    GeoDataFeature* feature = new GeoDataPlacemark();

    // Basic properties
    feature->setId( QString("%1").arg( agent->getId() ) );
    if( !agent->objectName().isEmpty() ){ feature->setName( agent->objectName() ); }
    feature->setDescription( QString("%1 %2 %3").arg( agent->objectName() ).arg( agent->metaObject()->className() ).arg( agent->getId() ) );
    feature->setZoomLevel( agent->getStyle()->zoom_level * 100 );

    const GSSGeometry* agent_geom = agent->getGeometry();
    if( agent_geom ){
      //  dynamic_cast<GeoDataPlacemark*>( feature )->setGeometry( this->geometryToGeoDataGeometry( agent_geom ) );
        delete agent_geom;
    }

    GeoDataStyle::Ptr style = this->styleToGeoDataStyle( agent->getStyle() );
    feature->setStyle( style );

    return feature;
}
