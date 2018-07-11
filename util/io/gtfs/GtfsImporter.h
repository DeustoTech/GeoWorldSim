#ifndef GTFSIMPORTER_H
#define GTFSIMPORTER_H

#include <QMap>
#include <QList>
#include <QTime>

#include "utils/io/ImporterExporter.h"

#include "agents/transport_networks/public_transport/PublicTransportStretch.h"
#include "agents/constructions/civil_structures/public_transport_stops/PublicTransportStop.h"
#include "agents/constructions/civil_structures/public_transport_stops/BusStop.h"
#include "agents/constructions/civil_structures/public_transport_stops/RailwayStop.h"
#include "agents/people/vehicle_drivers/PublicTransportVehicleDriver.h"

class GtfsImporter : public GSSImporterExporter
{
    Q_OBJECT
public:

    explicit GtfsImporter(QString files_folder_path , GWSObject* parent = Q_NULLPTR);
    ~GtfsImporter();

    // HELPER STRUCTS
    // https://developers.google.com/transit/gtfs/reference
    struct Agency {
        QString     agency_id; // Is key
        QString     agency_name;
        QString     agency_url;
        QString     agency_timezone;
        QString     agency_lang;
        QString     agency_phone;
        QString     agency_fare_url;
    };

    struct Stop {
        QString     stop_id; // Is key
        QString     stop_code;
        QString     stop_name;
        QString     stop_desc;
        double      stop_lat;
        double      stop_lon;
        int         zone_id;
        QString     stop_url;
        int         location_type;
        QString     parent_station;
        QString     stop_timezone;
        QString     wheelchair_boarding;
    };

    struct Route {
        QString     route_id; // Is key
        QString     agency_id;
        QString     route_short_name;
        QString     route_long_name;
        QString     route_desc;
        int         route_type; // Describes de vehicle type
        QString     route_url;
        QString     route_color;
        QString     route_text_color;
    };

    struct Trip {
        QString     route_id;
        QString     service_id;
        QString     trip_id; // Is key
        QString     trip_headsign;
        QString     trip_short_name;
        int         direction_id;
        QString     block_id;
        QString     shape_id;
        int         wheelchair_accessible;
        int         bikes_allowed;
    };

    struct StopTime {
        QString     trip_id; // Is key for grouping them
        QTime       arrival_time;
        QTime       departure_time;
        QString     stop_id;
        int         stop_sequence;
        QString     stop_headsign;
        int         pickup_type;
        int         drop_off_type;
        double      shape_dist_traveled;
        int         timepoint;
    };

    struct Calendar {
        QString     service_id; // Is key
        bool        monday;
        bool        tuesday;
        bool        wednesday;
        bool        thursday;
        bool        friday;
        bool        saturday;
        bool        sunday;
        QDate       start_date;
        QDate       end_date;
    };

    struct CalendarDate {
        QString     service_id; // Is key
        QDate       date;
        int         exception_type;
    };

    struct Shape {
        QString     shape_id; // Is key for grouping them
        double      shape_pt_lat;
        double      shape_pt_lon;
        int         shape_pt_sequence;
        double      shape_dist_traveled;
    };

    // GETTERS
    QList<GtfsImporter::Agency> getAgencies();
    Agency getAgencyById(QString id);

    QList<GtfsImporter::Stop> getStops();
    Stop getStopById(QString id);

    QList<GtfsImporter::Route> getRoutes();
    Route getRouteById(QString id);

    QList<GtfsImporter::Trip> getTrips();
    Trip getTripById(QString id);

    QList<GtfsImporter::StopTime> getStopTimes();
    QList<GtfsImporter::StopTime> getStopTimesByTripId(QString id);

    QList<GtfsImporter::Calendar> getCalendars();
    Calendar getCalendarByServiceId(QString id);

    QList<GtfsImporter::CalendarDate> getCalendarDates();
    QList<GtfsImporter::CalendarDate> getCalendarDatesByServiceId(QString id);

    QList<GtfsImporter::Shape> getShapes();
    QList<GtfsImporter::Shape> getShapesByShapeId(QString id);

    // METHODS
    void createStretchsInEnvironment( QDateTime from = QDateTime::currentDateTime() , QDateTime to = QDateTime::currentDateTime().addDays(1) );
    QList<PublicTransportVehicleDriver*> getGTFSDrivers( QDateTime from = QDateTime::currentDateTime() , QDateTime to = QDateTime::currentDateTime().addDays(1) );

private:

    // MAPS
    QMap<QString , GtfsImporter::Agency> agencies;
    QMap<QString , GtfsImporter::Stop> stops;
    QMap<QString , GtfsImporter::Route> routes;
    QMap<QString , GtfsImporter::Trip> trips;
    QMap<QString , QList<GtfsImporter::StopTime> > stoptimes;
    QMap<QString , GtfsImporter::Calendar> calendars;
    QMap<QString , QList<GtfsImporter::CalendarDate> > calendardates;
    QMap<QString , QList<GtfsImporter::Shape> > shapes;

    // METHODS
    QString getRouteVehicleType( GtfsImporter::Route route );
    QList<GtfsImporter::StopTime> getOrderedStopTimes( GtfsImporter::Trip trip );
    PublicTransportStop* snapToStopInEnvironment( GtfsImporter::Stop stop , QString vehicle_type );
};

#endif // GTFSIMPORTER_H
