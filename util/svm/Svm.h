#ifndef SVM_H
#define SVM_H

#include "../../../GeoWorldSim/util/ai/Intelligence.h"

#include <libsvm/svm.h>
#include <QJsonObject>


namespace geoworldsim {
namespace intelligence {


class Svm : public Intelligence
{
    Q_OBJECT

public:
    explicit Svm();
    ~Svm();

    // METHODS
    virtual void train( const QList< QMap< QString , QVariant> > &input_train_dataset, const QList< QMap< QString , QVariant> >  &output_train_dataset );
    virtual QJsonObject run( const QMap< QString, QVariant> &inputs );
    virtual void saveModel( QString model_file_path );
    virtual void loadModel( QString model_file_path );

private:

    svm_parameter parameters;
    svm_model *model = Q_NULLPTR;

    // Mutex, for avoiding concurrency
    mutable QMutex mutex;
};


}
}

#endif // SVM_H
