#ifndef GWSSOCIALENVIRONMENT_H
#define GWSSOCIALENVIRONMENT_H

#include "../../environment/Environment.h"


class GWSSocialEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSSocialEnvironment* globalInstance();

private:
    GWSSocialEnvironment();
    GWSSocialEnvironment(GWSSocialEnvironment const&);
    ~GWSSocialEnvironment();

    //QMap<QString, GWSGraph*> spatial_graph; // Social graphs

};

#endif // GWSSOCIALENVIRONMENT_H
