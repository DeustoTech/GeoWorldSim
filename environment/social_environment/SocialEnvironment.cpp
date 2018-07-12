#include <QDebug>

#include "SocialEnvironment.h"

#include "../../util/parallelism/ParallelismController.h"

GWSSocialEnvironment* GWSSocialEnvironment::globalInstance(){
    static GWSSocialEnvironment instance;
    return &instance;
}

GWSSocialEnvironment::GWSSocialEnvironment() : GWSEnvironment(){
    qInfo() << "SocialEnvironment created";
}

GWSSocialEnvironment::~GWSSocialEnvironment(){
}

