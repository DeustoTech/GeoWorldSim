#ifndef SVMSKILL_H
#define SVMSKILL_H

#include "../../skill/Skill.h"

#include "../../util/svm/Svm.h"
#include "../../util/storage/ObjectStorage.h"

namespace geoworldsim {
namespace skill {

class SVMSkill : public Skill
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SVMSkill();
    ~SVMSkill();

    void loadSVM( QString name , QString model_path , QString intputs_outputs_path );
    QSharedPointer< geoworldsim::intelligence::Svm > getSVM( QString name ) const;

private:

    storage::ObjectStorage* svms = Q_NULLPTR;

};

}
}

#endif // SVMSKILL_H
