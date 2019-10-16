#include "FindTSPRouteBehaviour.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../util/routing/TSPRouting.h"

QString FindTSPRouteBehaviour::ENTITY_TYPE_TO_ORDER = "entity_type_to_order";
QString FindTSPRouteBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";
QString FindTSPRouteBehaviour::STORE_TSP_ROUTE_IDS_AS = "store_tsp_ids_as";
QString FindTSPRouteBehaviour::STORE_TSP_ROUTE_DISTANCE_AS = "store_tsp_distance_as";
QString FindTSPRouteBehaviour::STORE_TSP_ROUTE_GEOM_AS = "store_tsp_geometry_as";
QString FindTSPRouteBehaviour::NEXTS = "nexts";
QString FindTSPRouteBehaviour::NEXTS_IF_NO_TSP_FOUND = "nexts_if_no_tsp_found";

FindTSPRouteBehaviour::FindTSPRouteBehaviour(){

}

QPair< double , QJsonArray > FindTSPRouteBehaviour::behave(){

    QString transport_network = this->getProperty( TRANSPORT_NETWORK_TYPE ).toString();
    if( transport_network.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_TSP_FOUND ).toArray() );
    }

    QString entity_type = this->getProperty( ENTITY_TYPE_TO_ORDER ).toString();
    if( entity_type.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_TSP_FOUND ).toArray() );
    }

    QList< QSharedPointer<GWSEntity> > entities_to_order = GWSEntityEnvironment::globalInstance()->getByClass( entity_type );
    if( entities_to_order.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_TSP_FOUND ).toArray() );
    }

    QSharedPointer<GWSEntity> entity = this->getEntity();
    GWSGeometry entity_geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    GWSCoordinate entity_coor = entity_geom.getCentroid();

    QMap< QString , GWSCoordinate> visits;
    foreach (QSharedPointer<GWSEntity> e, entities_to_order) {
        GWSGeometry entity_geom = GWSGeometry( e->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        if( entity_geom.isValid() ){
            visits.insert( e->getUID() , entity_geom.getCentroid() );
        }
    }

    GWSTSPRouting* tsp_routing = new GWSTSPRouting( transport_network );
    QList<GWSCoordinate> ordered_coors = tsp_routing->nearestNeighborTsp( entity_coor , visits.values() , entity_coor );
    qDebug() << ordered_coors.size();

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
