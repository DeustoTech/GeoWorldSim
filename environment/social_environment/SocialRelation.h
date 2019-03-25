#ifndef GWSSOCIALRELATION_H
#define GWSSOCIALRELATION_H

#include <QJsonObject>

#include "../../util/graph/Edge.h"

class GWSSocialRelation : GWSEdge {

    // PROPERTIES
    QString from_agent_uid;
    QString to_agent_uid;

    // JSON
    static QString AGENT_FROM_PROP;
    static QString AGENT_TO_PROP;
    static QString SOCIAL_RELATION_PROP;

    // CONSTRUCTORS
    GWSSocialRelation() : GWSEdge() {}
    GWSSocialRelation( QJsonObject json ) : GWSSocialRelation( json.value( AGENT_FROM_PROP ).toString() , json.value( AGENT_TO_PROP ).toString() , json.value( SOCIAL_RELATION_PROP ).toString() ) {}
    GWSSocialRelation( QString from , QString to , QString relation_type ) : GWSEdge( 1 , relation_type ) , from_agent_uid(from) , to_agent_uid(to) {}
    GWSSocialRelation(const GWSSocialRelation &other) : GWSSocialRelation(other.from_agent_uid , other.to_agent_uid , other.type ){}
    ~GWSSocialRelation(){}

    // GETTERS
    virtual QString getFromNodeUID() const;
    virtual QString getToNodeUID() const;

};

#endif // GWSSOCIALRELATION_H
