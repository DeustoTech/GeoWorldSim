#ifndef GWSGRIDENVIRONMENT_H
#define GWSGRIDENVIRONMENT_H

#include "../../environment/Environment.h"

#include "../../agent/Agent.h"
#include "../../util/grid/Grid.h"

class GWSGridEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSGridEnvironment* globalInstance();

    // PROPERTIES
    static QString GRID_PROP;

    const GWSGeometry getBounds() const;
    const GWSGeometry getBounds( QString class_name ) const;
    const QJsonValue getValue( QString class_name , GWSGeometry geom ) const;
    const QJsonValue getValue( QSharedPointer<GWSAgent> agent ) const;

    // METHODS
    virtual void registerAgent( QSharedPointer<GWSAgent> agent);
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent);

protected:
    void upsertValueToGrid( QSharedPointer<GWSAgent> agent , QJsonValue value );

protected slots:
    void agentPropertyChanged( QString property_name );

private:
    GWSGridEnvironment();
    GWSGridEnvironment(GWSGridEnvironment const&);
    ~GWSGridEnvironment();

    // SPATIAL INDEX
    QStringList environment_agent_grid_types;
    QMap< QString , QSharedPointer<GWSGrid> > environment_agent_grids; // Spatial indexes

    // Mutex, for avoiding concurrency
    QReadWriteLock mutex;

};

#endif // GWSGRIDENVIRONMENT_H
