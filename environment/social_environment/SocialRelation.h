#ifndef SOCIALRELATION_H
#define SOCIALRELATION_H

#include <QJsonObject>

#include "../../util/graph/Edge.h"

namespace geoworldsim {
namespace social {


struct SocialRelation : graph::Edge {

    // JSON
    static QString AGENT_FROM_PROP;
    static QString AGENT_TO_PROP;
    static QString SOCIAL_RELATION_PROP;

    // CONSTRUCTORS
    SocialRelation() : graph::Edge() {}
    SocialRelation( QJsonObject json ) : SocialRelation( json.value( AGENT_FROM_PROP ).toString() , json.value( AGENT_TO_PROP ).toString() , json.value( SOCIAL_RELATION_PROP ).toString() ) {}
    SocialRelation( QString from , QString to , QString relation_type ) : graph::Edge( 1 , relation_type , from , to ) {}
    ~SocialRelation(){}

};

}
}


#endif // SOCIALRELATION_H
