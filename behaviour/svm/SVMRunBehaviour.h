#ifndef SVMRUNBEHAVIOUR_H
#define SVMRUNBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class SVMRunBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SVMRunBehaviour();

    // PROPERTIES
    static QString INPUT_SVM_MODEL_PATH;
    static QString INPUT_SVM_INPUTS_OUTPUTS_PATH;
    static QString SVM_INPUTS; // Object with { inputname1 : value1 , inputname2 : value2 }
    static QString STORE_SVM_OUTPUTS_AS; // Object with { svmoutputname : storename }
    static QString NEXTS;

protected:

    // HOOKS
    virtual void afterCreateHook();

protected slots:
    virtual QPair< double , QJsonArray > behave();


};

}
}

#endif // SVMRUNBEHAVIOUR_H
