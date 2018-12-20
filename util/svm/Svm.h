#ifndef GWSSVM_H
#define GWSSVM_H

#include "../../../GeoWorldSim/util/ai/Intelligence.h"

#include <libsvm/svm.h>
#include <QJsonObject>

class GWSSvm : public GWSIntelligence
{
    Q_OBJECT

public:
    explicit GWSSvm();
    ~GWSSvm();

    // METHODS
    virtual void train( QList< QList< QPair< QString , QVariant> > > input_train_dataset, QList< QList< QPair< QString , QVariant> > >  output_train_dataset );
    virtual QJsonObject run(  QList<QPair <QString, QVariant> > inputs );

private:

    svm_parameter parameters;
    svm_model *model = Q_NULLPTR;

};

#endif // GWSSVM_H
