#ifndef GWSCUSTOMJSONCONVERSOR_H
#define GWSCUSTOMJSONCONVERSOR_H

#include <QObject>
#include <QJsonObject>
#include <QList>

#include "util/units/Units.h"
#include "util/geometry/Geometry.h"

QT_FORWARD_DECLARE_CLASS(GWSAgent)
QT_FORWARD_DECLARE_CLASS(GWSGrid)
QT_FORWARD_DECLARE_CLASS(GWSUiStyle)

class GWSCustomJsonConversor : public QObject
{
    Q_OBJECT
public:

    // SIMPLE ELEMENTS
    static QJsonObject formatVariant( QVariant value , QString units = "" );
    static QJsonObject formatUnit( GWSUnit value );
    static QJsonObject formatJsonValue( QJsonValue json );
    static QJsonObject formatJsonObject( QJsonObject json );
    static QJsonObject formatGeometry( GWSGeometry* geom , QJsonObject properties = QJsonObject() );

    // AGENT
    static QJsonObject formatAgent( GWSAgent* agent );
    static QJsonObject formatStyle( GWSUiStyle* style );

    // GRID
    static QJsonObject formatGrid( GWSGrid grid );

    // MAP
    static QJsonObject formatMap( QMap<QString , int> map , QString map_title = ""); // QMAP< QSTRING , DOUBLE > to BAR CHART
    static QJsonObject formatMap( QMap<QString , double> map , QString map_title = ""); // QMAP< QSTRING , DOUBLE > to BAR CHART
};

#endif // GWSCUSTOMJSONCONVERSOR_H
