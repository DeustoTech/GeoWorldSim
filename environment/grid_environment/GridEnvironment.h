#ifndef GWSGRIDENVIRONMENT_H
#define GWSGRIDENVIRONMENT_H

#include "../../environment/Environment.h"

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
    const QJsonValue getValue( QSharedPointer<GWSEntity> entity ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> entity);
    virtual void unregisterEntity( QSharedPointer<GWSEntity> entity);

protected:
    void upsertValueToGrid( QSharedPointer<GWSEntity> entity , const QJsonValue& value );

protected slots:
    void entityPropertyChanged( QString property_name );

private:
    GWSGridEnvironment();
    GWSGridEnvironment(GWSGridEnvironment const&);
    ~GWSGridEnvironment();

    // SPATIAL INDEX
    QMap< std::string , QSharedPointer<GWSGrid> > environment_entity_grids; // Spatial indexes

    // Mutex, for avoiding concurrency
    QReadWriteLock mutex;

};

#endif // GWSGRIDENVIRONMENT_H
