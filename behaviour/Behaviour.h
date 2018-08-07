#ifndef GWSBEHAVIOUR_H
#define GWSBEHAVIOUR_H

#include "../../object/Object.h"
#include "../../agent/Agent.h"

class GWSBehaviour : public GWSObject
{
    Q_OBJECT
    friend class GWSAgent; // Agent can check behaviour insides

public:
    Q_INVOKABLE explicit GWSBehaviour( GWSAgent* behaving_agent = Q_NULLPTR );

    // PROPERTIES
    static QString INCREMENT_AGENT_TIME_PROP; // In seconds
    static QString SUB_BEHAVIOURS_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    GWSAgent* getAgent();
    quint64 getBehavingTime() const;
    GWSBehaviour* getNext();
    virtual bool finished(); // Behaviour finished check

    // SETTERS
    void addSubbehaviour( GWSBehaviour* sub_behaviour );
    void setNextBehaviour( GWSBehaviour* next_behaviour );

protected:

    QList<GWSBehaviour*> sub_behaviours;
    GWSBehaviour* next_behaviour = Q_NULLPTR;

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    bool tick( qint64 behaviour_ticked_time ); // Acts as a behave() wrapper
    virtual bool behave(); // Behaviour, To be implemented by children, must be synchronous because tick() is already asyncrhonous

private:
    quint64 behaving_time = 0;
};

#endif // GWSBEHAVIOUR_H
