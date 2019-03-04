#ifndef GWSSOCIALENVIRONMENT_H
#define GWSSOCIALENVIRONMENT_H

#include "../../environment/Environment.h"
#include "SocialRelation.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Quadtree.h"

class GWSSocialEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSSocialEnvironment* globalInstance();

private:
    GWSSocialEnvironment();
    GWSSocialEnvironment(GWSSocialEnvironment const&);
    ~GWSSocialEnvironment();

    QMap< QString , QMap < QString , QList< GWSSocialRelation > > > agent_relations;

};

#endif // GWSSOCIALENVIRONMENT_H
