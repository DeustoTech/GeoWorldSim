#include "SendAgentSnapshotBehaviour.h"

QString SendAgentSnapshotBehaviour::NEXTS = "nexts";

SendAgentSnapshotBehaviour::SendAgentSnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QStringList SendAgentSnapshotBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;

}
