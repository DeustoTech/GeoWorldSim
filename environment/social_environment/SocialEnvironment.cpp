#include <QDebug>

#include "SocialEnvironment.h"

GWSSocialEnvironment* GWSSocialEnvironment::globalInstance(){
    static GWSSocialEnvironment instance;
    return &instance;
}

GWSSocialEnvironment::GWSSocialEnvironment() : GWSEnvironment(){
    qInfo() << "SocialEnvironment created";
}

GWSSocialEnvironment::~GWSSocialEnvironment(){
}

