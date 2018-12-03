#ifndef COMMUNICATIONENVIRONMENT_H
#define COMMUNICATIONENVIRONMENT_H

#include "../../environment/Environment.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/distributed/ExternalPublisher.h"

class GWSCommunicationEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    GWSCommunicationEnvironment();

private:

    QMap<QString , GWSExternalListener*> listeners;
    QMap<QString , GWSExternalPublisher*> publishers;

};

#endif // COMMUNICATIONENVIRONMENT_H
