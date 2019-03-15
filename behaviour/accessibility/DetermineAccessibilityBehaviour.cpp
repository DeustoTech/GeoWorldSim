#include "DetermineAccessibilityBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

QString DetermineAccessibilityBehaviour::FACILITY_TO_ACCESS = "facility_to_access";
QString DetermineAccessibilityBehaviour::MAXIMUM_ACCESSIBILITY_DISTANCE = "maximum_accessible_distance";
QString DetermineAccessibilityBehaviour::NETWORK_TO_ACCESS = "network";
QString DetermineAccessibilityBehaviour::NEXT = "next";

DetermineAccessibilityBehaviour::DetermineAccessibilityBehaviour() : GWSBehaviour ()
{

}



/************************************************************
 * SLOTS
 ***********************************************************/

QJsonArray DetermineAccessibilityBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    QString facility_to_access = this->getProperty( FACILITY_TO_ACCESS ).toString();
    QList<QSharedPointer<GWSAgent> > agents_to_access = GWSAgentEnvironment::globalInstance()->getByClass( facility_to_access );

    // Obtain coordinates of class of interest:
    QList<GWSCoordinate> allAgentsCoordinates;
    QMap< QString , GWSCoordinate > allAgentsCoorsIds;
    foreach (QSharedPointer<GWSAgent> agent, agents_to_access ){

        GWSCoordinate coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
        allAgentsCoordinates.append( coor );

        QString id = agent->getId();
        allAgentsCoorsIds.insert( id, coor );

    }

    QString network_class = this->getProperty( NETWORK_TO_ACCESS ).toString();

    QMap< QList<QSharedPointer< GWSNetworkEdge> > ,  GWSCoordinate > list;

    foreach ( GWSCoordinate coor, allAgentsCoordinates ){
        QList<QSharedPointer<GWSNetworkEdge> > min_path = GWSNetworkEnvironment::globalInstance()->getShortestPath( agent_coor , coor , network_class );
        list.insert( min_path , coor  );
    }


    GWSLengthUnit maximum_accessibility_distance = this->getProperty( MAXIMUM_ACCESSIBILITY_DISTANCE ).toDouble();

    QList< QString > closest_agent_id_list;

    foreach ( QList<QSharedPointer< GWSNetworkEdge> > path, list.keys() ){

        GWSLengthUnit l = 0;

        foreach ( QSharedPointer<GWSNetworkEdge> edge , path ){
            GWSCoordinate start = edge->getFromCoordinate();
            GWSCoordinate end = edge->getToCoordinate();
            l = l + start.getDistance( end );
        }


        if ( l <= maximum_accessibility_distance ){
            qDebug() << "Element within limits!";
           // paths_and_lengths.insert( path , l );
            GWSCoordinate coordinate = list.value( path );
            QString id = allAgentsCoorsIds.key( coordinate );
            closest_agent_id_list.append( id );
            // How do I retrieve the final coordinate of the path?
            //qDebug() << l.value;
        }
        else{
            qDebug() << "Too far!";
            qDebug() << l.value;
            continue;
        }
    }

    QList<QSharedPointer<GWSAgent>> closest_agent_list = GWSAgentEnvironment::globalInstance()->getByIds( closest_agent_id_list );

    foreach ( QSharedPointer<GWSAgent> a, closest_agent_list ){
        a->setProperty( "color" , "green" );
    }


    /*QList<QList<QSharedPointer< GWSNetworkEdge> > > shortestPaths = GWSNetworkEnvironment::globalInstance()->getShortestPaths( agent_coor, allAgentsCoordinates , network_class );
    QMap< QList<QSharedPointer< GWSNetworkEdge> > ,  GWSLengthUnit > paths_and_lengths;



    foreach ( QList<QSharedPointer< GWSNetworkEdge> > path , shortestPaths ){
        GWSLengthUnit l = 0;

        foreach ( QSharedPointer<GWSNetworkEdge> edge , path ){
            GWSCoordinate start = edge->getFrom();
            GWSCoordinate end = edge->getTo();
            l = l + start.getDistance( end );
        }

        if ( l <= maximum_accessibility_distance ){
            qDebug() << "Element within limits!";
            paths_and_lengths.insert( path , l );
            // How do I retrieve the final coordinate of the path?
            qDebug() << l.value;
        }
        else{
            qDebug() << "Too far!";
            qDebug() << l.value;
            continue;
        }

    }

    // How can I get the agents back from the shortest path map?

    // QJsonArray ordered_agents_to_visit_tsp_route_id_array;
    foreach( QList<QSharedPointer< GWSNetworkEdge> > path , paths_and_lengths.keys() ){
    //QString id = agents_to_visit_coord_id_array.key( coor );
    //ordered_agents_to_visit_tsp_route_id_array.append( id );
}*/


    return this->getProperty( NEXT ).toArray();
}
