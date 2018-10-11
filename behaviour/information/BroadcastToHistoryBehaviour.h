#ifndef BROADCASTTOHISTORYBEHAVIOUR_H
#define BROADCASTTOHISTORYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../util/api/APIDriver.h"
#include "../../app/App.h"

class BroadcastToHistoryBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit BroadcastToHistoryBehaviour();

    virtual bool canContinueToNext();

private slots:

    virtual bool behave();
    //GWSAPIDriver api_driver;
};

#endif // BROADCASTTOHISTORYBEHAVIOUR_H
