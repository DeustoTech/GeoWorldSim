#ifndef GWSGRIDENVIRONMENT_H
#define GWSGRIDENVIRONMENT_H

#include "../../environment/Environment.h"

#include "../../util/grid/Grid.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/units/Units.h"

class GWSGridEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSEnvironment; // Environment can access

public:
    static GWSGridEnvironment* globalInstance();

    // EXPORTERS
    //QJsonObject toJSON();
    //void fromJSON(QJsonObject json);

    // GETTERS


protected:
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSGridEnvironment();
    GWSGridEnvironment(GWSGridEnvironment const&);
    ~GWSGridEnvironment();


    QMap<QString, GWSGrid*> grids; // Grids
};

#endif // GWSGRIDENVIRONMENT_H
