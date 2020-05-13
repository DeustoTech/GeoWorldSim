#ifndef GRIDENVIRONMENT_H
#define GRIDENVIRONMENT_H

#include "../../environment/Environment.h"

#include "../../util/grid/Grid.h"

namespace geoworldsim {
namespace environment {


class GridEnvironment : public Environment
{
    Q_OBJECT

public:
    static GridEnvironment* globalInstance();

    // PROPERTIES
    static QString GRID_PROP;

    const geometry::Geometry getBounds() const;
    const geometry::Geometry getBounds( QString class_name ) const;
    const QJsonValue getValue( QString class_name , geometry::Geometry geom ) const;
    const QJsonValue getValue( QSharedPointer<Entity> entity ) const;

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> entity);
    virtual void unregisterEntity( QSharedPointer<Entity> entity);

protected:
    void upsertValueToGrid( QSharedPointer<Entity> entity , const QJsonValue& value );

protected slots:
    void entityPropertyChanged( QString property_name );

private:
    GridEnvironment();
    GridEnvironment(environment::GridEnvironment const&);
    ~GridEnvironment();

    // SPATIAL INDEX
    QMap< std::string , QSharedPointer<grid::Grid> > environment_entity_grids; // Spatial indexes

    // Mutex, for avoiding concurrency
    QReadWriteLock mutex;

};

}
}

#endif // GRIDENVIRONMENT_H
