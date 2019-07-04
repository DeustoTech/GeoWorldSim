#ifndef GWSBEHAVIOUR_H
#define GWSBEHAVIOUR_H

#include <QStringList>

#include "../../object/Object.h"
#include "../../entity/Entity.h"

class GWSBehaviour : public GWSObject
{
    Q_OBJECT
    friend class GWSEntity; // Agent can check behaviour insides

public:
    Q_INVOKABLE explicit GWSBehaviour();

    // PROPERTIES
    static QString BEHAVIOUR_DURATION; // In seconds, IT WILL INCREMENT AGENTS INTERNAL DATETIME
    static QString SNAPSHOT_EVERY_TICKS; // 0 = do not send agent snapshot | 1 = send snapshot at every tick of this behaviour | n = send snapshot at every n repetition
    static QString SUB_BEHAVIOURS_PROP;
    static QString FINISH_CONDITION_PROP;
    static QString START_BEHAVIOUR_PROP;

    // IMPORTERS
    virtual void deserialize( const QJsonObject &json , QSharedPointer<GWSObject> behaving_entity );

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer<GWSEntity> getEntity() const;
    int getTickedAmount() const;
    QList< QSharedPointer<GWSBehaviour> > getSubs() const;
    virtual QJsonValue getProperty(const QString &name ) const; // GETS the behaviours property value or if enlosed in '<>' goes to fetch it from the agent
    virtual void setProperty( const QString& name , const QJsonValue& value );

    // SETTERS
    void addSubbehaviour( QSharedPointer<GWSBehaviour> sub_behaviour );

    // METHODS
    //virtual bool canContinueToNext(); // Behaviour finished check

protected:

    QList< QSharedPointer<GWSBehaviour> > sub_behaviours; // IMPORTANT! If one subbehaviour finishes, the entire behaviour has finished
    //QStringList next_behaviour_ids;

private slots: // SLOTS, always invoke them by SLOT, it will make to be executed in the agent's thread
    QPair< double , QJsonArray > tick( qint64 behaviour_ticked_time ); // Acts as a behave() wrapper
    virtual QPair< double , QJsonArray > behave(); // Behaviour, To be implemented by children, must be synchronous because tick() is already asynchronous

private:
    quint64 behaving_time = 0;
    quint64 ticked_amount = 0;
    QSharedPointer<GWSEntity> behaving_entity;
};

Q_DECLARE_METATYPE( QSharedPointer<GWSBehaviour> ) // REQUIRED TO BE USED IN SIGNAL AND SLOTS

#endif // GWSBEHAVIOUR_H
