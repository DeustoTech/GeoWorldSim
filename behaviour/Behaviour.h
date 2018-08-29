#ifndef GWSBEHAVIOUR_H
#define GWSBEHAVIOUR_H

#include "../../object/Object.h"
#include "../../agent/Agent.h"

class GWSBehaviour : public GWSObject
{
    Q_OBJECT
    friend class GWSAgent; // Agent can check behaviour insides

public:
    Q_INVOKABLE explicit GWSBehaviour();

    // PROPERTIES
    static QString INCREMENT_AGENT_TIME_PROP; // In milliseconds
    static QString SUB_BEHAVIOURS_PROP;
    static QString FINISH_CONDITION_PROP;
    static QString NEXT_BEHAVIOURS_PROP;
    static QString START_BEHAVIOUR_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer<GWSAgent> getAgent();
    QList< QSharedPointer<GWSBehaviour> > getSubs();
    QList< QSharedPointer<GWSBehaviour> > getNext();
    virtual bool finished(); // Behaviour finished check

    // SETTERS
    void addSubbehaviour( QSharedPointer<GWSBehaviour> sub_behaviour );
    void addNextBehaviour( QSharedPointer<GWSBehaviour> next_behaviours );

protected:

    QList< QSharedPointer<GWSBehaviour> > sub_behaviours; // IMPORTANT! If one subbehaviour finishes, the entire behaviour has finished
    QList< QSharedPointer<GWSBehaviour> > next_behaviours;

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    bool tick( qint64 behaviour_ticked_time ); // Acts as a behave() wrapper
    virtual bool behave(); // Behaviour, To be implemented by children, must be synchronous because tick() is already asyncrhonous

private:
    quint64 behaving_time = 0;
};

#endif // GWSBEHAVIOUR_H
