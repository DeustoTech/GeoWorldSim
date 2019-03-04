#include "SocialEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

GWSSocialEnvironment* GWSSocialEnvironment::globalInstance(){
    static GWSSocialEnvironment instance;
    return &instance;
}

GWSSocialEnvironment::GWSSocialEnvironment() : GWSEnvironment(){
    qInfo() << "SocialEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSSocialEnvironment::~GWSSocialEnvironment(){
}


/**********************************************************************
 GETTERS
**********************************************************************/
